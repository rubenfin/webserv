/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   HttpHandler.hpp                                    :+:    :+:            */
/*                                                     +:+                    */
/*   By: jade-haa <jade-haa@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/13 20:00:39 by jade-haa      #+#    #+#                 */
/*   Updated: 2024/08/03 12:13:21 by rfinneru      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "HttpStatusCodes.hpp"
#include "Webserv.hpp"
#include <iostream>
#include <string>

class	Locations;
class	Server;
struct request_t;
struct response_t;

class HttpHandler
{
  private:
	int _idx;
	request_t *_request;
	response_t *_response;
	Locations *_foundDirective;
	Server *_server;
	bool _isCgi;
	bool _hasRedirect;
	bool _returnAutoIndex;
	bool _headerChecked;

  public:
	HttpHandler();
	~HttpHandler();

	void handleRequestBody(void);
	void checkRequestData(void);
	void combineRightUrl(void);
	void handleRequest(Server &serverAddresss);
	void httpVersionCheck(void);
	int pathCheck(void);
	void methodCheck(void);
	void fileCheck();
	void setDelete();
	void checkLocationMethod(void);
	Locations *getFoundDirective(void);
	request_t *getRequest(void);
	response_t *getResponse(void);
	Server *getServer(void);
	Locations *findMatchingDirective(void);
	void setBooleans(void);
	bool getCgi(void);
	bool getRedirect(void);
	bool getReturnAutoIndex(void);
	bool getHeaderChecked(void);
	void connectToRequestResponse(request_t *request, response_t *response, int idx);
	void cleanHttpHandler();
};
