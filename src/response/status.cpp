/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   status.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: rfinneru <rfinneru@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/07/31 16:16:47 by rfinneru      #+#    #+#                 */
/*   Updated: 2024/07/31 16:23:44 by rfinneru      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Webserv.hpp"

bool	positiveStatusCode(httpStatusCode status)
{
	return (status == httpStatusCode::OK || status == httpStatusCode::Created
		|| status == httpStatusCode::Accepted || status == httpStatusCode::Found
		|| status == httpStatusCode::NoContent
		|| status == httpStatusCode::MovedPermanently
		|| status == httpStatusCode::NotModified);
}

bool	negativeStatusCode(httpStatusCode status)
{
	return (status == httpStatusCode::BadRequest
		|| status == httpStatusCode::Unauthorized
		|| status == httpStatusCode::Forbidden
		|| status == httpStatusCode::NotFound
		|| status == httpStatusCode::MethodNotAllowed
		|| status == httpStatusCode::Conflict
		|| status == httpStatusCode::InternalServerError
		|| status == httpStatusCode::NotImplemented
		|| status == httpStatusCode::BadGateway
		|| status == httpStatusCode::ServiceUnavailable
		|| status == httpStatusCode::httpVersionNotSupported);
}
