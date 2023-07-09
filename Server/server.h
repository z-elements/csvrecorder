#pragma once

#include <array>
#include <iostream>
#include <fstream>
#include <string>
#include <memory>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time.hpp>
#include <../Common/logger.h>

struct store
{

    std::string rawvalue;
    double caldivition;
    std::string date;
    long long key;
};


class Session
    : public std::enable_shared_from_this<Session>
{
    static const int MaxLength = 4096;

public:
    using TcpSocket = boost::asio::ip::tcp::socket;

    Session(TcpSocket t_socket);

    void start()
    {
        count_lines=0;
        doJob();
    }

private:
    void doJob();
    void processRead(const boost::system::error_code& error, size_t bytesTransferred);
    void handleError(std::string const& functionName, boost::system::error_code const& ec);
    void writeHandler(const boost::system::error_code &error, const size_t bytesTransferred);
    int parseRow(const std::string &s, char delim);
    time_t toTime(boost::posix_time::ptime t);
    TcpSocket m_socket;

    std::array<char, MaxLength> m_buf;
    store max_date;
    long long count_lines;
};


class Server
{
public:
    using TcpSocket = boost::asio::ip::tcp::socket;
    using TcpAcceptor = boost::asio::ip::tcp::acceptor;
    using IoService = boost::asio::io_service;

    Server(IoService& t_ioService, short t_port);

private:
    void doAccept();

    TcpSocket m_socket;
    TcpAcceptor m_acceptor;
    std::string m_workDirectory;
};
