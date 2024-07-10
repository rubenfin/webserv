/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Request.hpp                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: rfinneru <rfinneru@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/24 17:36:08 by rfinneru      #+#    #+#                 */
/*   Updated: 2024/07/10 09:20:42 by ruben         ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Webserv.hpp"
#include "File.hpp"
#include <map>
#include <string.h>
#include <string>
#include <sys/stat.h>

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
	std::string requestDirectory;
	std::string requestFile;
	std::string contentType;
	int 		contentLength;
	file_t 		file;
	METHODS	method;
	std::map<std::string, std::string> header;
}			request_t;

void 			setFile(request_t *req, file_t *requestFile);
void			parse_request(request_t *req, char *buffer);
void			resetRequest(request_t &request);
void			printRequestStruct(request_t *req);
std::string 	trim(const std::string &str);
std::string 	extractValue(request_t *req, const std::string &toSearch);
