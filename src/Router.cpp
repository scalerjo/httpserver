#include "Router.h"

std::string Resource1 = "./views/index.html";
std::string Empty = "";
std::string Error = "-1";

std::string& Router::Get(const HttpRequest& Request){
    
    const std::string& URL = Request.target().to_string();
    if (URL[0] != '/' || URL.find("..") != std::string::npos){
        return Error;
    }
    else if (URL.compare("/")==0){
        return Resource1;
    }
    return Empty;
}

std::string& Router::Post(const HttpRequest& Request){
    const std::string& URL = Request.target().to_string();
    return Resource1;
}