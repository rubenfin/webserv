/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpHandler.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jade-haa <jade-haa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/13 20:00:39 by jade-haa          #+#    #+#             */
/*   Updated: 2024/07/31 14:19:44 by jade-haa         ###   ########.fr       */
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
	request_t *_request;
	response_t *_response;
	Locations *_foundDirective;
	Server *_server;
	int _readCount;
	bool _isCgi;
	bool _hasRedirect;
	bool _returnAutoIndex;

  public:
	HttpHandler();
	~HttpHandler();

	void handleRequestBody(void);
	void checkRequestData(void);
	void combineRightUrl(void);
	void handleRequest(Server &serverAddresss, request_t *request,
		response_t *response);
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
	void setReadCount(int ReadCount);
	int getReadCount(void);
	bool getCgi(void);
	bool getRedirect(void);
	bool getReturnAutoIndex(void);
};
