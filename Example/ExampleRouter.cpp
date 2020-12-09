#include "../Router.h"

std::string home = "./public/index.html";

std::string& Router::Get(const HttpRequest& Request, std::stack<std::string>& CookieStack){
    //Return a Path to a local html file. Return an empty string for invalid URL.
    return home;
}

std::string& Router::Post(const HttpRequest& Request){
    //Currently does nothing.
    return home;
}
