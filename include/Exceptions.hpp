/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Exceptions.hpp                                     :+:    :+:            */
/*                                                     +:+                    */
/*   By: rfinneru <rfinneru@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/25 11:26:08 by rfinneru      #+#    #+#                 */
/*   Updated: 2024/08/09 11:47:00 by rfinneru      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#ifndef EXCEPTIONS_HPP
#define EXCEPTIONS_HPP

#include "HttpHandler.hpp"
#include <exception>
#include <stdexcept>
#include <string>

class HttpException : public std::runtime_error {
protected:
    int statusCode;
    char * pageContent;

public:
    HttpException(int code, const std::string& message, char *content)
        : std::runtime_error(message), statusCode(code), pageContent(content) {}

    int getStatusCode() const { return statusCode; }
    char * getPageContent() const { return pageContent; }
};

#define DEFINE_HTTP_EXCEPTION(name, code, content) \
class name ## Exception : public HttpException { \
public: \
    name ## Exception() : HttpException(code, #name, content) {} \
};


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
DEFINE_HTTP_EXCEPTION(InternalServerError, 500, PAGE_500)
DEFINE_HTTP_EXCEPTION(NotImplemented, 501, PAGE_501)
DEFINE_HTTP_EXCEPTION(BadGateway, 502, PAGE_502)
DEFINE_HTTP_EXCEPTION(ServiceUnavailable, 503, PAGE_503)
DEFINE_HTTP_EXCEPTION(HttpVersionNotSupported, 505, PAGE_505)

extern void makeResponse(char *buffer, int index);

#endif

