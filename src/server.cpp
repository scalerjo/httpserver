#include "HttpServerBuilder.h"
#include "Router.cpp"

/**
 * Purpose: Print command line argument usage.
 * @returns void
 */
void PrintHelp()
{
    std::cout << "Usage: server <address> <port> <doc_root> <num_workers> {spin|block}\n";
    std::cout << "  For IPv4, try:\n";
    std::cout << "    server 0.0.0.0 80 . 100 block\n";
    std::cout << "  For IPv6, try:\n";
    std::cout << "    server 0::0 80 . 100 block\n";
}


int main(int argc, char *argv[])
{
    // Check command line arguments.
    if (argc != 6){
        PrintHelp();
        return 1;
    }

    try
    {
        //Create our defined router
        Router MyRouter;

        //Reference ServerStartDetails.h for Argument Params
        ServerStartDetails SSD(argv);

        //Pass our router and server settings to the server builder
        ServerBuilder Server(MyRouter, SSD);

        //Start the server!
        Server.Start();
    }
    catch (const boost::bad_lexical_cast &blc)
    {
        std::cerr << "Error parsing server start port or number of workers [" << blc.what() << "]" << std::endl;
    }
    

    return 0;
}
