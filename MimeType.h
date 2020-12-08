#include <boost/beast/core.hpp>
#include <string>


/**
 * Purpose: Strip the input string Path to the file's extension
 * @returns String, a file extension
 */
const std::string GetExtensionFromFilename(const boost::beast::string_view &Path)
{
    std::string result("");

    if (!Path.empty())
    {
        auto pos(Path.rfind("."));
        if (pos != boost::beast::string_view::npos)
        {
            result = Path.substr(pos).to_string();
        }
    }

    return result;
}

/**
 * Return a reasonable mime type based on the extension of a file.
 * @param Path  string_view from boost beast library
 * @returns string_view, mime type
 */
const boost::beast::string_view GetMimeType(const boost::beast::string_view &Path)
{
    boost::beast::string_view result("application/text");

    const auto ext(GetExtensionFromFilename(Path));

    using boost::beast::iequals;
    if (iequals(ext, ".html"))
        result = "text/html";
    else if (iequals(ext, ".htm"))
        result = "text/html";
    else if (iequals(ext, ".cpp"))
        result = "text/html";
    else if (iequals(ext, ".h"))
        result = "text/html";
    else if (iequals(ext, ".hpp"))
        result = "text/html";
    else if (iequals(ext, ".php"))
        result = "text/html";
    else if (iequals(ext, ".css"))
        result = "text/css";
    else if (iequals(ext, ".txt"))
        result = "text/plain";
    else if (iequals(ext, ".js"))
        result = "application/javascript";
    else if (iequals(ext, ".json"))
        result = "application/json";
    else if (iequals(ext, ".xml"))
        result = "application/xml";
    else if (iequals(ext, ".swf"))
        result = "application/x-shockwave-flash";
    else if (iequals(ext, ".flv"))
        result = "video/x-flv";
    else if (iequals(ext, ".png"))
        result = "image/png";
    else if (iequals(ext, ".jpe"))
        result = "image/jpeg";
    else if (iequals(ext, ".jpeg"))
        result = "image/jpeg";
    else if (iequals(ext, ".jpg"))
        result = "image/jpeg";
    else if (iequals(ext, ".gelse if"))
        result = "image/gelse if";
    else if (iequals(ext, ".bmp"))
        result = "image/bmp";
    else if (iequals(ext, ".ico"))
        result = "image/vnd.microsoft.icon";
    else if (iequals(ext, ".telse iff"))
        result = "image/telse iff";
    else if (iequals(ext, ".telse if"))
        result = "image/telse iff";
    else if (iequals(ext, ".svg"))
        result = "image/svg+xml";
    else if (iequals(ext, ".svgz"))
        result = "image/svg+xml";

    return result;
}