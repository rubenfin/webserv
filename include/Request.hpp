/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Request.hpp                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: rfinneru <rfinneru@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/24 17:36:08 by rfinneru      #+#    #+#                 */
/*   Updated: 2024/06/24 18:43:26 by rfinneru      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Webserv.hpp"
#include <map>
#include <string.h>
#include <string>

typedef enum METHODS
{
	GET,
	POST,
	DELETE,
	ERROR
}			METHODS;

typedef struct request_t
{
	std::string http_v;
	std::string firstLine;
	std::string requestContent;
	std::string requestBody;
	std::string requestURL;
	METHODS	method;
	std::map<std::string, std::string> header;
}			request_t;

void		parse_request(request_t *r, char *buffer);
void		resetRequest(request_t &request);
void		printRequestStruct(request_t *r);
std::string trim(const std::string &str);