/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   HttpHandler.hpp                                    :+:    :+:            */
/*                                                     +:+                    */
/*   By: jade-haa <jade-haa@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/13 20:00:39 by jade-haa      #+#    #+#                 */
/*   Updated: 2024/06/21 17:10:00 by rfinneru      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "HttpStatusCodes.hpp"
#include "Webserv.hpp"
#include <iostream>
#include <string>

struct				MethodsReq
{
	bool			GET;
	bool			POST;
	bool			DELETE;
} typedef MethodsReq;

class				Locations;
class				Server;

class HttpHandler
{
  private:
	httpStatusCode	_statusCode;
	std::string 	_httpVersion;
	std::string 	_responseContent;
	std::string		_requestContent;
	std::string 	_requestBody;
	std::string		_request;
	std::string 	_response;
	MethodsReq		_allowedMethods;
	std::string 	_requestURL;
	Locations		*_foundDirective;
	Server			*_server;
	std::string 	_contentType;
	std::string 	_boundary;
	uint64_t		_contentLength;
	bool			_isCgi;

  public:
	HttpHandler();
	~HttpHandler();
	std::string extractValue(const std::string &toSearch);
	void setResponseContent(std::string content);
	void setRequest();
	void setRequestBody(void);
	void storeRequestBody(void);
	void setMethods(void);
	void setData(void);
	void setDataContent(void);
	void setHttpVersion(void);
	void setBoundary(void);
	void setContentType(void);
	void setContentLength(void);
	void setHttpStatusCode(httpStatusCode code);

	void combineRightUrl(void);
	void handleRequest(const std::string &content, Server &serverAddress);
	std::string getHttpStatusMessage() const;
	std::string getHttpVersion(void);
	httpStatusCode getHttpStatusCode(void) const;
	std::string getRequestBody(void);
	std::string getResponseContent(void);
	std::string getResponseURL(void);
	std::string getContentType(void);
	Locations *getFoundDirective(void);
	uint64_t getContentLength(void);
	bool 	getCgi(void);
	// std::string setRequestContent(void);
	std::string findRequestedURL(const std::string &content);
	Locations *findMatchingDirective(void);

	// const std::vector<Server>& getServers() const;
};
