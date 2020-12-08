#include "../HttpServerBuilder.h"
#include "ExampleRouter.cpp"

int main(int argc, char *argv[])
{
    //WARNING, You are responsible for inputting command line arguments and checking their validity!

    //Create our defined router
    Router MyRouter;

    //Reference ServerStartDetails.h for Argument Params
    ServerStartDetails SSD(argv);

    //Pass our router and server settings to the server builder
    ServerBuilder Server(MyRouter, SSD);

    //Start the server!
    Server.Start();
    
    return 0;
}
