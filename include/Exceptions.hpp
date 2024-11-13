/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Exceptions.hpp                                     :+:    :+:            */
/*                                                     +:+                    */
/*   By: rfinneru <rfinneru@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/08/19 15:12:06 by rfinneru      #+#    #+#                 */
/*   Updated: 2024/11/13 18:21:39 by rfinneru      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#ifndef EXCEPTIONS_HPP
#define EXCEPTIONS_HPP

#include "HTTPHandler.hpp"
#include <string>
#include <unordered_map>
#include <cstring>
#include <memory>
#include <stdexcept>

// Base HttpException class
class HttpException : public std::runtime_error {
protected:
    int statusCode;
    char * pageContent;

    static std::unordered_map<std::string, std::shared_ptr<char[]>> customPages;

public:
    HttpException(int code, const std::string& message, char *defaultContent);
    char * getPageContent() const;

    static void setCustomPage(const std::string& exceptionName, const char* content);
};

// Macro to define specific HTTP exceptions
#define DEFINE_HTTP_EXCEPTION(name, code, defaultContent) \
class name ## Exception : public HttpException { \
public: \
    name ## Exception() : HttpException(code, #name, defaultContent) {} \
};

// Define specific HTTP exceptions with default content
DEFINE_HTTP_EXCEPTION(Created, 201, PAGE_201)
DEFINE_HTTP_EXCEPTION(Accepted, 202, PAGE_202)
DEFINE_HTTP_EXCEPTION(NoContent, 204, PAGE_204)
DEFINE_HTTP_EXCEPTION(BadRequest, 400, PAGE_400)
DEFINE_HTTP_EXCEPTION(Unauthorized, 401, PAGE_401)
DEFINE_HTTP_EXCEPTION(Forbidden, 403, PAGE_403)
DEFINE_HTTP_EXCEPTION(NotFound, 404, PAGE_404)
DEFINE_HTTP_EXCEPTION(FavIcon, 404, PAGE_404)
DEFINE_HTTP_EXCEPTION(MethodNotAllowed, 405, PAGE_405)
DEFINE_HTTP_EXCEPTION(Conflict, 409, PAGE_409)
DEFINE_HTTP_EXCEPTION(PayloadTooLarge, 413, PAGE_413)
DEFINE_HTTP_EXCEPTION(InternalServerError, 500, PAGE_500)
DEFINE_HTTP_EXCEPTION(NotImplemented, 501, PAGE_501)
DEFINE_HTTP_EXCEPTION(BadGateway, 502, PAGE_502)
DEFINE_HTTP_EXCEPTION(ServiceUnavailable, 503, PAGE_503)
DEFINE_HTTP_EXCEPTION(HttpVersionNotSupported, 505, PAGE_505)


#endif
