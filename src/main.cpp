#include "server.h"
//-----------------------------------------------------------------------------
int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;

    try
    {
        boost::asio::io_service ios;
        Server s(ios, 23071);
        ios.run();
    }
    catch (std::exception& e)
    {
        std::cout << e.what() << std::endl;
    }
    return EXIT_SUCCESS;
}
//-----------------------------------------------------------------------------
