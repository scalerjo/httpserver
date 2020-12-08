#ifndef HTTP_SERVER_FAST
#define HTTP_SERVER_FAST

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/asio.hpp>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <chrono>
#include <cstring>
#include <iostream>
#include <memory>
#include <string>
#include <sstream>
#include "CookieHandler.h"
#include "ServerStartDetails.h"
#include "FieldsAlloc.h"
#include "MimeType.h"
#include "Router.h"

using alloc_t = fields_alloc<char>;
using request_body_t = boost::beast::http::basic_dynamic_body<boost::beast::flat_static_buffer<Mbyte>>;

using ParserType =
    boost::optional<
        boost::beast::http::request_parser<
            request_body_t,
            alloc_t>>;

using StringResponseType =
    boost::optional<
        boost::beast::http::response<
            boost::beast::http::string_body,
            boost::beast::http::basic_fields<alloc_t>>>;

using StringSerializerTye =
    boost::optional<
        boost::beast::http::response_serializer<
            boost::beast::http::string_body,
            boost::beast::http::basic_fields<alloc_t>>>;

using FileResponseType =
    boost::optional<
        boost::beast::http::response<
            boost::beast::http::file_body,
            boost::beast::http::basic_fields<alloc_t>>>;

using FileSerializerType =
    boost::optional<
        boost::beast::http::response_serializer<
            boost::beast::http::file_body,
            boost::beast::http::basic_fields<alloc_t>>>;

class HttpWorker;
using HttpWorkerPtr = std::shared_ptr<HttpWorker>;
typedef boost::beast::http::request<request_body_t, boost::beast::http::basic_fields<alloc_t>> HttpRequest;

class HttpWorker
{
private:
    HttpWorker() = delete;
    HttpWorker(const HttpWorker &) = delete;
    HttpWorker &operator=(const HttpWorker &) = delete;
    HttpWorker(
        boost::asio::ip::tcp::acceptor &Acceptor,
        const std::string &DocumentRoot,
        Router &Router)
        : Router_(Router),
          Acceptor(Acceptor),
          DocumentRoot(DocumentRoot),
          Socket({Acceptor.get_executor()}),
          Alloc(8192),
          RequestDeadline(
              {Acceptor.get_executor(),
               (std::chrono::steady_clock::time_point::max)()})
    {
    }

public:
    static HttpWorkerPtr Create(
        boost::asio::ip::tcp::acceptor &Acceptor,
        const std::string &DocumentRoot,
        Router &Router)
    {
        return HttpWorkerPtr(
            new HttpWorker(
                Acceptor,
                DocumentRoot,
                Router));
    }

    void Start()
    {
        Accept();
        CheckDeadline();
    }

private:
    // The router used to route endpoints
    Router &Router_;

    // The acceptor used to listen for incoming connections.
    boost::asio::ip::tcp::acceptor &Acceptor;

    // The path to the root of the document directory.
    std::string DocumentRoot;

    // The socket for the currently connected client.
    boost::asio::ip::tcp::socket Socket;

    // The buffer for performing reads
    boost::beast::flat_static_buffer<8192> Buffer;

    // The allocator used for the fields in the request and reply.
    alloc_t Alloc;

    // The parser for reading the requests
    ParserType Parser;

    // The timer putting a time limit on requests.
    boost::asio::basic_waitable_timer<std::chrono::steady_clock> RequestDeadline;

    // The string-based response message.
    StringResponseType StringResponse;

    // The string-based response serializer.
    StringSerializerTye StringSerializer;

    // The file-based response message.
    FileResponseType FileResponse;

    // The file-based response serializer.
    FileSerializerType FileSerializer;

    /**
     * Purpose: Accept socket connection
     * @returns void
     */
    void Accept()
    {
        // Clean up any previous connection.
        boost::beast::error_code ec;
        Socket.close(ec);
        Buffer.consume(Buffer.size());

        Acceptor.async_accept(
            Socket,
            [this](boost::beast::error_code ec) {
                if (ec)
                {
                    Accept();
                }
                else
                {
                    // Request must be fully processed within 60 seconds.
                    RequestDeadline.expires_after(
                        std::chrono::seconds(60));

                    ReadRequest();
                }
            });
    }

    /**
     * 1MB limit to prevent buffer attacks
     * @returns void
     */
    void ReadRequest()
    {
        // On each read the parser needs to be destroyed and
        // recreated. We store it in a boost::optional to
        // achieve that.

        // Arguments passed to the parser constructor are
        // forwarded to the message object. A single argument
        // is forwarded to the body constructor.

        Parser.emplace(
            std::piecewise_construct,
            std::make_tuple(),
            std::make_tuple(Alloc));

        boost::beast::http::async_read(
            Socket,
            Buffer,
            *Parser,
            [this](boost::beast::error_code ec, std::size_t) {
                if (ec)
                {
                    Accept();
                }
                else
                {
                    ProcessRequest(Parser->get());
                }
            });
    }
    /**
     * Purpose: Process the Request method. Throw Bad Response Status if unknown method type.
     * @param   Request     Boost Beast http Request structure
     * @returns void
     */
    void ProcessRequest(const HttpRequest &Request)
    {
        //std::cout << Request[boost::beast::http::field::cookie] << std::endl;
        switch (Request.method())
        {
        case boost::beast::http::verb::get:
            RouteEndpoint(Request);
            break;

        default:
            SendBadResponse(
                boost::beast::http::status::bad_request,
                "Invalid request-method '" + Request.method_string().to_string() + "'\r\n");
            break;
        }
    }
    /**
     * Purpose: Sends a bad response. Status Error.
     * @param Status    http status code
     * @param Error     String
     * @return void
     */
    void SendBadResponse(
        const boost::beast::http::status &Status,
        std::string const &Error)
    {
        StringResponse.emplace(
            std::piecewise_construct,
            std::make_tuple(),
            std::make_tuple(Alloc));

        StringResponse->result(Status);
        StringResponse->keep_alive(false);
        StringResponse->set(boost::beast::http::field::server, "Beast");
        StringResponse->set(boost::beast::http::field::content_type, "text/plain");
        StringResponse->body() = Error;
        StringResponse->prepare_payload();

        StringSerializer.emplace(*StringResponse);

        boost::beast::http::async_write(
            Socket,
            *StringSerializer,
            [this](boost::beast::error_code ec, std::size_t) {
                Socket.shutdown(boost::asio::ip::tcp::socket::shutdown_send, ec);
                StringSerializer.reset();
                StringResponse.reset();
                Accept();
            });
    }
    /**
     * Purpose: Sends a "File not found" message and a Bad Response status error
     * @returns void
     * */
    void SendBadFileResponse()
    {
        SendBadResponse(
            boost::beast::http::status::not_found,
            "File not found\r\n");
    }

    /**
     * Purpose: Route the Target resource Dynamically.
     * @param Target    Contains request information and target resource
     * @returns void
     */ 
    void RouteEndpoint(const HttpRequest& Request)
    {
        //Is empty() depricated? When is Target.empty() true?
        //if (Request.target().empty() || Request.target() != '/' || Target.find("..") != std::string::npos)

        std::string& resource = Router_.Get(Request);
        if (resource.compare("-1")==0){
            SendBadFileResponse();
        }
        else {
            if (resource.size()>0){
                std::string FullPath(resource);
                SendStaticFile(FullPath);
            }
            else {
                std::string FullPath(DocumentRoot);
                FullPath.append(
                    Request.target().data(),
                    Request.target().size());
                SendStaticFile(FullPath);
            }
        }
    }
    /**
     * Purpose: Full path to the desired file to send
     * @param string FullPath 
     * @returns void
     */
    void SendStaticFile(const std::string &FullPath)
    {
        boost::beast::error_code ec;
        boost::beast::http::file_body::value_type File;
        File.open(
            FullPath.c_str(),
            boost::beast::file_mode::read,
            ec);
        if (ec)
        {
            SendBadFileResponse();
        }
        else
        {
            FileResponse.emplace(
                std::piecewise_construct,
                std::make_tuple(),
                std::make_tuple(Alloc));

            FileResponse->result(boost::beast::http::status::ok);
            FileResponse->keep_alive(false);
            FileResponse->set(boost::beast::http::field::server, "Beast");
            FileResponse->set(boost::beast::http::field::content_type, GetMimeType(FullPath));
            FileResponse->set(boost::beast::http::field::set_cookie, MakeTestCookie("jwt", "hello"));
            FileResponse->body() = std::move(File);
            FileResponse->prepare_payload();

            FileSerializer.emplace(*FileResponse);
            boost::beast::http::async_write(
                Socket,
                *FileSerializer,
                [this](boost::beast::error_code ec, std::size_t) {
                    Socket.shutdown(boost::asio::ip::tcp::socket::shutdown_send, ec);
                    FileSerializer.reset();
                    FileResponse.reset();
                    Accept();
                });
        }
    }
    /**
     * Purpose: Enforces socket Expiration date
     * @returns void
     */
    void CheckDeadline()
    {
        // The deadline may have moved, so check it has really passed.
        if (RequestDeadline.expiry() <= std::chrono::steady_clock::now())
        {
            // Close socket to cancel any outstanding operation.
            boost::beast::error_code ec;
            Socket.close();

            // Sleep indefinitely until we're given a new deadline.
            RequestDeadline.expires_at(
                std::chrono::steady_clock::time_point::max());
        }

        RequestDeadline.async_wait(
            [this](boost::beast::error_code) {
                CheckDeadline();
            });
    }
};

#endif 