#pragma once

#include <array>
#include <fstream>
#include <boost/asio.hpp>
#include <iostream>
#include <memory>


class Client : public std::enable_shared_from_this<Client>
{
public:
    using IoService = boost::asio::io_service;
    using TcpResolver = boost::asio::ip::tcp::resolver;
    using TcpResolverIterator = TcpResolver::iterator;
    using TcpSocket = boost::asio::ip::tcp::socket;

    Client(IoService& t_ioService, TcpResolverIterator t_endpointIterator, 
        std::string const& t_path);

private:
    int openFile(std::string const& path);
    void doConnect(int fsize);
    void doRead();
    void handleError(const std::string &functionName, const boost::system::error_code &ec);
    void writeBuffer(int fsize);
    void doReadContent(size_t bytesTransferred, std::istream &streamdata);

    TcpResolver m_ioService;
    TcpSocket m_socket;
    TcpResolverIterator m_endpointIterator;
    std::string m_path;

    /* file processing*/
    std::vector<char> m_buf;
    std::ifstream m_sourceFile;
    /* file processing*/

};

