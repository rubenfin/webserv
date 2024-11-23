/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   HTTPHandler.hpp                                    :+:    :+:            */
/*                                                     +:+                    */
/*   By: jade-haa <jade-haa@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/13 20:00:39 by jade-haa      #+#    #+#                 */
/*   Updated: 2024/11/13 18:50:53 by rfinneru      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "HttpStatusCodes.hpp"
#include "Webserv.hpp"
#include <iostream>
#include <string>
#include <memory>
#include <unordered_map>
#include "Request.hpp"
#include "Response.hpp"
#include "File.hpp"

class	Locations;
class	Server;
struct request_t;
struct response_t;
struct CGI_t;

struct file_t;

typedef enum METHODS
{
	GET,
	POST,
	DELETE,
	ERROR
}			METHODS;

typedef struct response_t
{
	httpStatusCode	status;
	std::string response;
	int contentLength; 
	response_t() : status(httpStatusCode::OK), response(""), contentLength(0) {}
}					response_t;


typedef struct request_t
{
	bool	    internalError;
	bool		bin;
	bool		foundHeaderEnd;
	bool		favIcon;
	std::string http_v;
	std::string firstLine;
	std::string requestContent;
	std::string requestBody;
	std::string requestURL;
	std::string requestDirectory;
	std::string requestFile;
	std::string contentType;
	size_t		currentBytesRead;
	size_t		totalBytesRead;
	size_t		contentLength;
	file_t	file;
	METHODS	method;
	std::map<std::string, std::string> header;
	
	request_t() : internalError(false), bin(false), foundHeaderEnd(false), favIcon(false), http_v(""), firstLine(""), requestContent(""),
		requestBody(""), requestURL(""), requestDirectory(""),
		requestFile(""), contentType(""), currentBytesRead(0),
		totalBytesRead(0), contentLength(0), method(ERROR) {}
}			request_t;

class HTTPHandler
{
  private:
	int _connectedToSocket;
	CGI_t *_cgiPtr;
	int _idx;
	std::string _firstRequest;
	response_t _response;
	request_t _request; 
	std::shared_ptr<Locations>  _foundDirective;
	Server *_server;
	std::unordered_map<int, bool> *_socketReceivedFirstRequest;
	int _currentSocket;
	int _connectedToFile;
	long long _fileBytesRead;
	long long _totalToRead;
	bool _isCgi;
	bool _hasRedirect;
	bool _returnAutoIndex;
	bool _headerChecked;
	bool _isChunked;

  public:
	HTTPHandler();
	~HTTPHandler();

	void favIconCheck(void);
	void setIndex(const int& idx);
  	CGI_t * getConnectedToCGI(void);
  	void setConnectedToCGI(CGI_t *cgiPtr);
	void setConnectedToFile(const int &fd);
	int getConnectedToFile(void);
	void setFirstRequest(std::string buffer);
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
	request_t &getRequest(void);
	response_t &getResponse(void);
	Server *getServer(void);
	std::shared_ptr<Locations> findMatchingDirective(void);
	void headerTooLongCheck(void);
	void parsingErrorCheck(void);
	void setCurrentSocket(int fd);
	void setBooleans(void);
	bool getCgi(void);
	bool getRedirect(void);
	int getIdx(void);
	bool getReturnAutoIndex(void);
	bool getHeaderChecked(void);
	bool getChunked(void);
	void setFileBytesRead(long long size);
	void setTotalToRead(long long total);
	long long getFileBytesRead(void);
	long long getTotalToRead(void);
	void cleanHTTPHandler();
};
