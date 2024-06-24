/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Response.hpp                                       :+:    :+:            */
/*                                                     +:+                    */
/*   By: rfinneru <rfinneru@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/24 18:06:37 by rfinneru      #+#    #+#                 */
/*   Updated: 2024/06/24 18:47:47 by rfinneru      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "HttpStatusCodes.hpp"
#include "Webserv.hpp"
#include "Colors.hpp"

typedef struct response_t
{
	httpStatusCode	status;
	std::string response;

}					response_t;

void				resetResponse(response_t &response);
std::string getHttpStatusMessage(httpStatusCode _statusCode);