#include <iostream>

#include <boost/asio/read_until.hpp>
#include <boost/filesystem.hpp>
#include <boost/log/trivial.hpp>
#include "server.h"


Session::Session(TcpSocket t_socket)
    : m_socket(std::move(t_socket))
{
}

void Session::doJob()
{
    m_socket.async_read_some(
            boost::asio::buffer(m_buf, MaxLength),
            boost::bind(&Session::processRead,
            shared_from_this(),
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));

}


void Session::processRead(const boost::system::error_code& error, size_t bytesTransferred)
{
    std::cout << "Bytes recieved: " << bytesTransferred << std::endl;
    if (!error)
    {
        if (bytesTransferred>0)
        {
            Log::Logger::getInstance()->writelogprotocol("Server mess recv  "+std::to_string(bytesTransferred)+ " bytes");
            string str=string(m_buf.data(), bytesTransferred);
            str=str.substr(3,str.length()-1);
            int status = parseRow(str, ';');
            if (status == -1)
            {
                m_socket.close();
                return;

            } else
            {
            Log::Logger::getInstance()->writelogprotocol("Server mess Date= "+max_date.date+" calc division="+std::to_string(max_date.caldivition));
            string clientmess= "count :"+std::to_string(count_lines);
            m_socket.async_write_some(
                                boost::asio::buffer(&clientmess[0],  clientmess.length()),
                                boost::bind(&Session::writeHandler,
                                shared_from_this(),
                                boost::asio::placeholders::error,
                                boost::asio::placeholders::bytes_transferred));
            }
        }

    } else
    {
        string outerr="could not write: "+ string(boost::system::system_error(error).what());
        Log::Logger::getInstance()->writelogprotocol(outerr);
        std::cerr << outerr<< std::endl;
        m_socket.close();
        return;
    };

}


time_t Session::toTime(boost::posix_time::ptime t)
{
    using namespace boost::posix_time;
    ptime epoch(boost::gregorian::date(1970,1,1));
    time_duration::sec_type x = (t - epoch).total_seconds();
    return time_t(x);
}

int  Session::parseRow(const std::string &s, char delim) {
    std::stringstream ss (s);
    std::string item;

    static const boost::regex xRegEx("((?:\n)*)\"([0-3][0-9].[0-1][0-9].(?:(?:19|20)[0-9]{2}) [0-3][0-9]:[0-3][0-9]:[0-6][0-9])\":([0-9]+.[0-9]+):([0-9]+.[0-6][0-9])");
    boost::smatch xResults;

    static const std::locale loc = std::locale(std::locale::classic(), new boost::posix_time::time_input_facet("%d.%m.%Y %H:%M:%S"));
    while (getline (ss, item, delim)&&item.length()>2)
    {

        if (boost::regex_match(item,  xResults, xRegEx))
        {
            std::istringstream is(xResults[2].str());
            is.imbue(loc);
            boost::posix_time::ptime t;
            is >> t;
            time_t key=toTime(t);
            double a = std::stod(xResults[3]);
            double b = std::stod(xResults[4]);
            double res = b>0 ? a / b : 0;
            if (max_date.key<key)
                max_date = {item,res,xResults[2].str(),key};
            count_lines++;

        } else {
            const string err="Can`t parse string. Wrong format of string";
            std::cerr << err<< std::endl;
            Log::Logger::getInstance()->writelogprotocol(err);
            return -1;
        }
    }
    return 0;
}

void Session::writeHandler(const boost::system::error_code& error, const size_t bytesTransferred)
{
    if (!error)
    {
        Log::Logger::getInstance()->writelogprotocol("Server mess bytes send to client "+std::to_string(bytesTransferred));

    } else  {
        string outerr="could not write: "+ string(boost::system::system_error(error).what());
        std::cerr <<  outerr << std::endl;
        Log::Logger::getInstance()->writelogprotocol(outerr);
        m_socket.close();
        return;
    }

}


void Session::handleError(std::string const& functionName, boost::system::error_code const& ec)
{
    Log::Logger::getInstance()->writelogprotocol("Server mess Error in "+functionName+"due to"+std::to_string(ec.value())+" "+ec.message());
}


Server::Server(IoService& t_ioService, short t_port)
    : m_socket(t_ioService),
      m_acceptor(t_ioService, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), t_port))
{
    Log::Logger::getInstance()->writelogprotocol("Server started");

    doAccept();
}


void Server::doAccept()
{
    m_acceptor.async_accept(m_socket,
                            [this](boost::system::error_code ec)
    {
        if (!ec)
            std::make_shared<Session>(std::move(m_socket))->start();

        doAccept();
    });
}


