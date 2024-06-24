/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   parseRequest.cpp                                   :+:    :+:            */
/*                                                     +:+                    */
/*   By: rfinneru <rfinneru@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/24 16:12:04 by rfinneru      #+#    #+#                 */
/*   Updated: 2024/06/24 18:37:13 by rfinneru      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Request.hpp"

void	setHttpVersion(request_t *r)
{
	std::size_t firstSpace = r->firstLine.find(" ");
	if (firstSpace == std::string::npos)
	{
		r->http_v = "invalid";
		return ;
	}
	std::size_t secondSpace = r->firstLine.find(" ", firstSpace + 1);
	if (secondSpace == std::string::npos)
	{
		r->http_v = "invalid";
		return ;
	}
	std::string httpVersion = r->firstLine.substr(secondSpace + 1);
	httpVersion = trim(httpVersion);
	if (httpVersion == "HTTP/1.1")
	{
		r->http_v = "1.1";
	}
	else
	{
		r->http_v = "invalid";
	}
}

void	setMethod(request_t *r)
{
	if (r->firstLine.find("GET") != std::string::npos)
		r->method = GET;
	else if (r->firstLine.find("POST") != std::string::npos)
		r->method = POST;
	else if (r->firstLine.find("DELETE") != std::string::npos)
		r->method = DELETE;
	else
		r->method = ERROR;
	// TODO: Throw an exception if no method found?
}

void	setRequestURL(request_t *r)
{
	std::size_t startPos = r->firstLine.find(" ");
	if (startPos == std::string::npos)
	{
		r->requestURL = "";
		return ;
	}
	startPos = r->firstLine.find("/", startPos);
	if (startPos == std::string::npos)
	{
		r->requestURL = "";
		return ;
	}
	std::size_t endPos = r->firstLine.find(" ", startPos);
	if (endPos == std::string::npos)
		r->requestURL = r->firstLine.substr(startPos);
	else
		r->requestURL = r->firstLine.substr(startPos, endPos - startPos);
}

void	setRequestHeader(request_t *r)
{
	std::size_t startPos = r->requestContent.find("\n") + 1;
	std::size_t endPos;
	while ((endPos = r->requestContent.find("\n",
				startPos)) != std::string::npos)
	{
		std::string headerLine = r->requestContent.substr(startPos, endPos
				- startPos);
		if (headerLine.find(":") == std::string::npos)
		{
			break ;
		}
		std::size_t delimiterPos = headerLine.find(":");
		std::string key = headerLine.substr(0, delimiterPos);
		std::string value = headerLine.substr(delimiterPos + 1);
		key.erase(0, key.find_first_not_of(" \t"));
		key.erase(key.find_last_not_of(" \t") + 1);
		value.erase(0, value.find_first_not_of(" \t"));
		value.erase(value.find_last_not_of(" \t") + 1);
		r->header[key] = value;
		startPos = endPos + 1;
	}
}

void	setRequestBody(request_t *r)
{
	std::size_t foundBody = r->requestContent.find("\r\n\r\n");
	if (foundBody == std::string::npos)
	{
		r->requestBody = "";
		return ;
	}
	foundBody += 4;
	r->requestBody = r->requestContent.substr(foundBody);
}

void	parse_request(request_t *r, char *buffer)
{
	r->requestContent = buffer;
	r->firstLine = r->requestContent.substr(0, r->requestContent.find("\n"));
	setHttpVersion(r);
	setMethod(r);
	setRequestURL(r);
	setRequestHeader(r);
	setRequestBody(r);
	printRequestStruct(r);
}