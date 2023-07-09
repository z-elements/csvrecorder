#include <string>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem.hpp>
#include <../Common/logger.h>

#include "client.h"


Client::Client(IoService& t_ioService, TcpResolverIterator t_endpointIterator, 
               std::string const& t_path)
    : m_ioService(t_ioService), m_socket(t_ioService),
      m_endpointIterator(t_endpointIterator), m_path(t_path)
{
    int fsize=openFile(m_path);
    if (fsize<=0)
    {
        return;
    }
    doConnect(fsize);
    doRead();
}

void Client::doRead()
{
    boost::system::error_code error_code;
    boost::asio::streambuf receive_buffer;
    int bytesTransferred=boost::asio::read(m_socket, receive_buffer, boost::asio::transfer_all(), error_code);
    if( error_code && error_code != boost::asio::error::eof ) {
        Log::Logger::getInstance()->writelogprotocol("receive failed: " +error_code.message());
        cout << "receive failed: " << error_code.message() << endl;
    }
    else {
        std::string serveranswer((std::istreambuf_iterator<char>(&receive_buffer)),std::istreambuf_iterator<char>());
        if (serveranswer!="")
        {
            std::cout<<"server mess "<<serveranswer<<std::endl;
            Log::Logger::getInstance()->writelogprotocol(serveranswer);
        }
        m_socket.close();

    }
}

void Client::handleError(std::string const& functionName, boost::system::error_code const& ec)
{
    std::cout<<"Error in "<<functionName<<" due to"<<std::to_string(ec.value())<<" "<<ec.message()<<std::endl;
    Log::Logger::getInstance()->writelogprotocol("Error in "+functionName+"due to"+std::to_string(ec.value())+" "+ec.message());
}



int Client::openFile(std::string const& path)
{
    m_sourceFile.open(path, std::ios_base::binary);
    if (m_sourceFile.fail())
    {
        m_sourceFile.close();
        Log::Logger::getInstance()->writelogprotocol("Failed while opening file " + path);
    }
    bool isEmpty = m_sourceFile.peek() == EOF;
    m_sourceFile.seekg(0, m_sourceFile.end);
    long fileSize = m_sourceFile.tellg();
    if (fileSize<=0||isEmpty) {
        std::cout << "Bad file size"<<std::endl;
        Log::Logger::getInstance()->writelogprotocol("Client Error: Bad file size");
        m_socket.close();
        m_sourceFile.close();
        return -1;

    }
    m_buf.resize(fileSize);
    m_sourceFile.seekg(0, m_sourceFile.beg);
    m_sourceFile.read(&m_buf[0], fileSize);
    Log::Logger::getInstance()->writelogprotocol("Request size: "+ std::to_string(fileSize));
    m_sourceFile.close();
    return fileSize;
}


void Client::doConnect(int fsize)
{
    boost::system::error_code ec;
    boost::asio::connect(m_socket, m_endpointIterator, ec);
    if (!ec) {
        writeBuffer(fsize);
    } else {
        std::cout << "Coudn't connect to host. Please run server "
                     "or check network connection.\n";
        Log::Logger::getInstance()->writelogprotocol("Error: " + ec.message());
    }
}

void Client::writeBuffer(int fsize)
{
    boost::system::error_code ec;
    size_t bytesSended = boost::asio::write(m_socket,  boost::asio::buffer(m_buf.data(),fsize), ec);
    std::cout << "Bytes sended: " << bytesSended << std::endl;
    if (ec)
    {
        Log::Logger::getInstance()->writelogprotocol("Error: " + ec.message());
        return;
    }
}

