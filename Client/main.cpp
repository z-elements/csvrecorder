#include <iostream>
#include <boost/asio.hpp>
#include <boost/asio/io_service.hpp>
#include "client.h"
#include <array>
#include <fstream>

int main(int argc, char* argv[])
{
    if (argc != 4) {
        std::cerr << "Usage: client <address> <port> <filePath>\n";
        return 1;
    }


    auto address = argv[1];
    auto port = argv[2];
    auto filePath = argv[3];

    try {
        boost::asio::io_service ioService;

        boost::asio::ip::tcp::resolver resolver(ioService);
        auto endpointIterator = resolver.resolve({ address, port });
        Client client(ioService, endpointIterator, filePath);
        ioService.run();


    } catch (std::fstream::failure& e) {
        std::cerr << e.what() << "\n";
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
