/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   HTTPHandler.hpp                                    :+:    :+:            */
/*                                                     +:+                    */
/*   By: jade-haa <jade-haa@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/13 20:00:39 by jade-haa      #+#    #+#                 */
/*   Updated: 2024/12/12 11:57:04 by rfinneru      ########   odam.nl         */
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
#include "FileDescriptor.hpp"

class	Locations;
class	Server;
struct request_t;
struct response_t;
struct CGI_t;
class FileDescriptor;
struct SessionData;

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
	std::string responseHeader;
	std::string response;
	int contentLength; 
	response_t() : status(httpStatusCode::OK), responseHeader(""), response(""), contentLength(0) {}
}					response_t;

class FileDescriptor {
private:
    int _fd;
    int64_t _totalToRead;
    int64_t _bytesRead;
    bool _isOpen;
    HTTPHandler* _connectedToHandler;

public:
    FileDescriptor();
    FileDescriptor(const FileDescriptor&);
    FileDescriptor& operator=(const FileDescriptor&) ;
    FileDescriptor(FileDescriptor&& other) noexcept ;
    FileDescriptor& operator=(FileDescriptor&& other) noexcept;
    void setFileFd(int fd);
    void setTotalToRead(int64_t total);
    void setHandler(HTTPHandler* handler);
    HTTPHandler* getHandler(void);
    void incrementBytesRead(int64_t bytes);
    int getFileFd() const ;
    int64_t getTotalToRead() const ;
    int64_t getBytesRead() const;
    bool isOpen() const;
    bool isReadComplete() const;
    void clean();
    ~FileDescriptor();
};


typedef struct request_t
{
	bool		badRequest;
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
	std::string cookie;
	size_t		currentBytesRead;
	size_t		totalBytesRead;
	size_t		contentLength;
	file_t	file;
	METHODS	method;
	std::map<std::string, std::string> header;
	
	request_t() : badRequest(false), internalError(false), bin(false), foundHeaderEnd(false), favIcon(false), http_v(""), firstLine(""), requestContent(""),
		requestBody(""), requestURL(""), requestDirectory(""),
		requestFile(""), contentType(""), currentBytesRead(0),
		totalBytesRead(0), contentLength(0), method(ERROR) {}
}			request_t;

class HTTPHandler
{
  private:
	int _connectedToSocket;
	CGI_t *_cgiPtr;
	FileDescriptor _FDs;
	int _idx;
	std::string _firstRequest;
	response_t _response;
	request_t _request; 
	std::shared_ptr<Locations>  _foundDirective;
	Server *_server;
	std::unordered_map<int, bool> *_socketReceivedFirstRequest;
	int _currentSocket;
	bool _isCgi;
	bool _hasRedirect;
	bool _returnAutoIndex;
	bool _headerChecked;
	bool _isChunked;
	std::shared_ptr<SessionData> _currentSession;

  public:
	HTTPHandler();
	HTTPHandler(HTTPHandler &&other) noexcept;
    HTTPHandler &operator=(HTTPHandler &&other) noexcept;
	HTTPHandler(const HTTPHandler&);
    HTTPHandler& operator=(const HTTPHandler&);
	~HTTPHandler();

	std::shared_ptr<SessionData> getCurrentSession();
	void checkForSession();
	void favIconCheck(void);
	void createNewSession();
	FileDescriptor* getFDs(void);
	void setIndex(const int& idx);
  	CGI_t * getConnectedToCGI(void);
  	void setConnectedToCGI(CGI_t *cgiPtr);
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
	void cleanHTTPHandler();
};
