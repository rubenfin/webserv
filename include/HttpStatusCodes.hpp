/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   HttpStatusCodes.hpp                                :+:    :+:            */
/*                                                     +:+                    */
/*   By: rfinneru <rfinneru@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/18 14:28:52 by rfinneru      #+#    #+#                 */
/*   Updated: 2024/08/14 15:40:23 by rfinneru      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_STATUS_CODES_HPP
#define HTTP_STATUS_CODES_HPP

#define PAGE_201 (char *)"<h1>201 - Created</h1>\n<h2>Your file has been successfully uploaded to the server.</h2>\n<p><a href=\"/\">Return to homepage</a></p>"
#define PAGE_202 (char *)"<h1>202 - Accepted </h1>"
#define PAGE_204 (char *)"<h1>204 - No Content </h1>"
#define PAGE_400 (char *)"<h1>400 - Bad Request </h1>"
#define PAGE_401 (char *)"<h1>401 - Unauthorized </h1>"
#define PAGE_403 (char *)"<h1>403 - Forbidden </h1>"
#define PAGE_404 (char *)"<h1>404 - Not Found </h1>"
#define PAGE_405 (char *)"<h1>405 - Method Not Allowed </h1>"
#define PAGE_409 (char *)"<h1>409 - Conflict </h1>"
#define PAGE_500 (char *)"<h1>500 - Internal Server Error </h1>"
#define PAGE_501 (char *)"<h1>501 - Not Implemented </h1>"
#define PAGE_502 (char *)"<h1>502 - Bad Gateway </h1>"
#define PAGE_503 (char *)"<h1>503 - Service Unaivalable </h1>"
#define PAGE_505 (char *)"<h1>505 - HTTP Version Not Supported </h1>"

enum class httpStatusCode {
    OK = 200,
    Created = 201,
    Accepted = 202,
    NoContent = 204,
    MovedPermanently = 301,
    Found = 302,
    NotModified = 304,
    BadRequest = 400,
    Unauthorized = 401,
    Forbidden = 403,
    NotFound = 404,
    MethodNotAllowed = 405,
    Conflict = 409,
    InternalServerError = 500,
    NotImplemented = 501,
    BadGateway = 502,
    ServiceUnavailable = 503,
    httpVersionNotSupported = 505
};

bool positiveStatusCode(httpStatusCode status);
bool negativeStatusCode(httpStatusCode status);


#endif