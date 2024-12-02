/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   fillResponse.cpp                                   :+:    :+:            */
/*                                                     +:+                    */
/*   By: rfinneru <rfinneru@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/24 18:18:07 by rfinneru      #+#    #+#                 */
/*   Updated: 2024/12/02 18:21:19 by rfinneru      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Response.hpp"

void resetResponse(response_t &response)
{
    response.status = httpStatusCode::OK;
	response.responseHeader = "";
	response.response = "";
	response.contentLength = 0;
}

std::string getHttpStatusMessage(httpStatusCode _statusCode)
{
    switch (_statusCode)
    {
    case httpStatusCode::OK:
        return ("200 OK");
    case httpStatusCode::Created:
        return ("201 Created");
    case httpStatusCode::Accepted:
        return ("202 Accepted");
    case httpStatusCode::NoContent:
        return ("204 No Content");
    case httpStatusCode::MovedPermanently:
        return ("301 Moved Permanently");
    case httpStatusCode::Found:
        return ("302 Found");
    case httpStatusCode::NotModified:
        return ("304 Not Modified");
    case httpStatusCode::BadRequest:
        return ("400 Bad Request");
    case httpStatusCode::Unauthorized:
        return ("401 Unauthorized");
    case httpStatusCode::Forbidden:
        return ("403 Forbidden");
    case httpStatusCode::NotFound:
        return ("404 Not Found");
    case httpStatusCode::MethodNotAllowed:
        return ("405 Method Not Allowed");
    case httpStatusCode::Conflict:
        return ("409 Conflict");
    case httpStatusCode::PayloadTooLarge:
        return ("413 Payload Too Large");
    case httpStatusCode::InternalServerError:
        return ("500 Internal Server Error");
    case httpStatusCode::NotImplemented:
        return ("501 Not Implemented");
    case httpStatusCode::BadGateway:
        return ("502 Bad Gateway");
    case httpStatusCode::ServiceUnavailable:
        return ("503 Service Unavailable");
    case httpStatusCode::httpVersionNotSupported:
        return ("505 HTTP Version Not Supported");
    default:
        return ("500 Internal Server Error");
    }
}

char *getHttpStatusHTML(httpStatusCode _statusCode)
{
    switch (_statusCode)
    {
    case httpStatusCode::NoContent:
        return ((char *)PAGE_204);
    case httpStatusCode::BadRequest:
        return ((char *)PAGE_400);
    case httpStatusCode::Unauthorized:
        return ((char *)PAGE_401);
    case httpStatusCode::Forbidden:
        return ((char *)PAGE_403);
    case httpStatusCode::NotFound:
        return ((char *)PAGE_404);
    case httpStatusCode::MethodNotAllowed:
        return ((char *)PAGE_405);
    case httpStatusCode::Conflict:
        return ((char *)PAGE_409);
    case httpStatusCode::PayloadTooLarge:
        return ((char *)PAGE_413);
    case httpStatusCode::InternalServerError:
        return ((char *)PAGE_500);
    case httpStatusCode::NotImplemented:
        return ((char *)PAGE_501);
    case httpStatusCode::BadGateway:
        return ((char *)PAGE_502);
    case httpStatusCode::ServiceUnavailable:
        return ((char *)PAGE_503);
    case httpStatusCode::httpVersionNotSupported:
        return ((char *)PAGE_505);
    default:
        return ((char *)PAGE_500);
    }
}

