#include "server.h"
//-----------------------------------------------------------------------------
int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cout << "You did not specified a port" << std::endl;
        return EXIT_FAILURE;
    }

    try
    {
        int port = std::stoi(argv[1]);

        boost::asio::io_service ios;
        Server s(ios, port);
        ios.run();
    }
    catch (std::exception& e)
    {
        std::cout << e.what() << std::endl;
    }
    return EXIT_SUCCESS;
}
//-----------------------------------------------------------------------------
