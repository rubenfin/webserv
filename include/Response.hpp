/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Response.hpp                                       :+:    :+:            */
/*                                                     +:+                    */
/*   By: rfinneru <rfinneru@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/24 18:06:37 by rfinneru      #+#    #+#                 */
/*   Updated: 2024/09/17 11:39:09 by rfinneru      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "HttpStatusCodes.hpp"
#include "Webserv.hpp"
#include "Colors.hpp"
#include "HTTPHandler.hpp"
#include <string>

struct response_t;


void				resetResponse(response_t &response);
std::string			getHttpStatusMessage(httpStatusCode _statusCode);
char *				getHttpStatusHTML(httpStatusCode _statusCode);