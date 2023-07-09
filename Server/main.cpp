#include <iostream>

#include <boost/asio/io_service.hpp>

#include "server.h"


int main(int argc, char* argv[])
{
    if (argc != 2) {
        std::cerr << "Usage: server <port>\n";
        return 1;
    }

    try
    {
        boost::asio::io_service ioService;

        Server server(ioService, std::stoi(argv[1]));

        ioService.run();
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
