# Http Server
This project was build using the Boost Library. Please refer to License doc.

Download Boost library here: https://www.boost.org/users/download/

# Environment
This project was designed for linux systems. However, the project should work fine on Windows. The compile script might not be the same as what is listed below.

# Setup
## 1. Download and Build the Boost library using the link below.
Download Boost library here: https://www.boost.org/users/download/

## 2. Create a new file called Router.cpp. Start with this template.
```cpp
#include "Router.h"

std::string& Router::Get(const HttpRequest& Request){
    //Return a Path to a local html file. Return an empty string for invalid URL.
}

std::string& Router::Post(const HttpRequest& Request){
    //Currently does nothing.
}
```
do Request.target().to_string() to get a String representation of the URL.

Refer to the Boost doccumentation for all other methods for the Request Object.

Both methods must be defined for the server to work!

## 3. Create a new file called Server.cpp. Start with this template.
```cpp
#include "HttpServerBuilder.h"
#include "Router.cpp"

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
```
## 4. Use this to build.
g++ Server.cpp -o Server -lboost_thread -lpthread
Do not forget to include the path to your Boost directory.

## 5. Run the server.
Usage: IP PORT ROOTDIR WORKERS spin|block

./Server 0.0.0.0 3000 . 100 block
Use curl to perform a get request.

curl localhost:3000
# Warning
Refer to the License agreement before distributing or using
