#ifndef COOKIE_HANDLER
#define COOKIE_HANDLER

#include <string>
#include <time.h>

const std::string CookieFlags = "; Path=/; Secure=true; HttpOnly=true; SameSite=strict;";

/**
 * Purpose: Create the Set-Cookie string for the http response with a 15 minute experation date 
 * @param name  string
 * @param value string
 * @returns     string
 */
std::string MakeTestCookie(const std::string& value){
    char buf[1000];
    size_t min = 15;
    time_t now = time(0) + (60 * min);
    struct tm tm = *gmtime(&now);
    strftime(buf, sizeof buf, "%a, %d %b %Y %H:%M:%S %Z", &tm);
    return "jwt=" + value + "; Expires=" + buf + CookieFlags;
}

#endif