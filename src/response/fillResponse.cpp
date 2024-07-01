/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   fillResponse.cpp                                   :+:    :+:            */
/*                                                     +:+                    */
/*   By: rfinneru <rfinneru@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/24 18:18:07 by rfinneru      #+#    #+#                 */
/*   Updated: 2024/06/30 15:56:54 by rfinneru      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Response.hpp"

void resetResponse(response_t &response)
{
    response.status = httpStatusCode::OK;
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
