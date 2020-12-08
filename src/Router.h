#ifndef ROUTER_HEADER
#define ROUTER_HEADER

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <string>
#include "FieldsAlloc.h"

constexpr int Mbyte = 1024 * 1024;
using alloc_t = fields_alloc<char>;
using request_body_t = boost::beast::http::basic_dynamic_body<boost::beast::flat_static_buffer<Mbyte>>;
typedef boost::beast::http::request<request_body_t, boost::beast::http::basic_fields<alloc_t>> HttpRequest;

class Router {
    
    public:
        Router() {}
        
        /**
         * Purpose: Process the Get request.  
         * @param   Request HttpRequest
         * @returns URL     string
         */
        std::string& Get(const HttpRequest& Request);

        /**
         * Purpose: Process the Post request.  
         * @param   Request HttpRequest
         * @returns URL     string
         */
        std::string& Post(const HttpRequest& Request);

    
    private:
        std::vector<std::string> routes_;
};




#endif