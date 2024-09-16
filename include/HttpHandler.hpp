/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   HttpHandler.hpp                                    :+:    :+:            */
/*                                                     +:+                    */
/*   By: jade-haa <jade-haa@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/13 20:00:39 by jade-haa      #+#    #+#                 */
/*   Updated: 2024/09/16 16:49:33 by rfinneru      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "HttpStatusCodes.hpp"
#include "Webserv.hpp"
#include <iostream>
#include <string>
#include <memory>
#include <unordered_map>

class	Locations;
class	Server;
struct request_t;
struct response_t;
struct CGI_t;

class HttpHandler
{
  private:
	int _connectedToSocket;
	CGI_t *_cgiPtr;
	int _idx;
	std::string _firstRequest;
	request_t *_request; 
	response_t *_response;
	std::shared_ptr<Locations>  _foundDirective;
	Server *_server;
	std::unordered_map<int, bool> *_socketReceivedFirstRequest;
	int _currentSocket;
	bool _isCgi;
	bool _hasRedirect;
	bool _returnAutoIndex;
	bool _headerChecked;
	bool _isChunked;

  public:
	HttpHandler();
	~HttpHandler();

  	CGI_t * getConnectedToCGI(void);
  	void setConnectedToCGI(CGI_t *cgiPtr);
	void setFirstRequest(std::string buffer);
	void handleRequestBody(void);
	void checkRequestData(void);
	void combineRightUrl(void);
	void handleRequest(Server &serverAddresss);
	void httpVersionCheck(void);
	void totalPathCheck(void);
	int pathCheck(const std::string& dir, const std::string& file);
	void methodCheck(void);
	void fileCheck();
	void setDelete();
	void setHeaderChecked(bool boolean);
	void setChunked(bool boolean);
	void setConnectedToSocket(const int& fd);
	int& getConnectedToSocket(void);
	void checkLocationMethod(void);
	void checkClientBodySize();
	void linkToReceivedFirstRequest(std::unordered_map<int, bool> *_socketReceivedFirstRequest);
	std::shared_ptr<Locations>  getFoundDirective(void);
	request_t *getRequest(void);
	response_t *getResponse(void);
	Server *getServer(void);
	std::shared_ptr<Locations> findMatchingDirective(void);
	void setCurrentSocket(int fd);
	void setBooleans(void);
	bool getCgi(void);
	bool getRedirect(void);
	int getIdx(void);
	bool getReturnAutoIndex(void);
	bool getHeaderChecked(void);
	bool getChunked(void);
	void connectToRequestResponse(request_t *request, response_t *response, int idx);
	void cleanHttpHandler();
};
