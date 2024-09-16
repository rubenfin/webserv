/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Server.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: jade-haa <jade-haa@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/09 15:40:25 by jade-haa      #+#    #+#                 */
/*   Updated: 2024/09/16 13:57:01 by rfinneru      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Exceptions.hpp"
#include "HttpHandler.hpp"
#include "Locations.hpp"
#include "CGI.hpp"
#include "Webserv.hpp"
#include <fcntl.h>
#include <unordered_map>
#include <iostream>
#include <netinet/in.h>
#include <regex>
#include <sstream>
#include <vector>
#include <string>
#include <sys/socket.h>
#include <unordered_set>
#include <sys/epoll.h>

#define MAX_EVENTS 64

class					Locations;
class					HttpHandler;
class					HttpException;
struct					CGI_t;

struct					Methods
{
	bool				GET;
	bool				POST;
	bool				DELETE;
} typedef Methods;

class Server
{
  protected:
	std::unordered_map<int, Server*> *_connectedServersPtr;
	std::map<int, CGI_t*> _fdsRunningCGI;
	int*					_epollFDptr;
	std::string 			_serverContent;
	std::string 			_portString;
	std::string 			_methodsList;
	int						_serverFd;
	std::string 			_serverName;
	std::string				_serverNameIP;
	int						_port;
	std::string 			_root;
	std::string 			_index;
	std::string 			_upload;
	Methods					_allowedMethods;
	std::vector<Locations>  _locations;
	struct sockaddr_in		*_address;
	socklen_t				_addrlen;
	long long				_client_body_size_server;
	std::vector<HttpHandler> _http_handler;

  public:
	void readWriteCGI(int socket, HttpHandler& handler);
 	void removeCGIrunning(int socket);
	std::map<int, CGI_t*> &getFdsRunningCGI(void);
  	void removeSocketAndServer(int socket);
	void addFdToReadEpoll(epoll_event &eventConfig, int &socket);
	void readFromSocketError(const int &err, const int &idx, int &socket);
	int serverActions(const int &idx, int &socket);
  	void readFromSocketSuccess(const int &idx, const char *buffer,
	const int &bytes_read);
	void removeFdFromEpoll(int &socket);
	void setFdReadyForRead(epoll_event &eventConfig, int &socket);
	void setFdReadyForWrite(epoll_event &eventConfig, const int &socket);
	void readWriteServer(epoll_event& event,epoll_event& eventConfig, HttpHandler& handler);
	void cgi(int index, int socket);
	void execute_CGI_script(int *writeSide, int *readSide, const char *script, int idx);
	void getLocationStack(std::string locationContent);
	void logThrowStatus(const int &idx, const level &lvl,
		const std::string &msg, const httpStatusCode &status,
		HttpException exception);
	std::string extractValue(const std::string &searchString);
	std::string extractValueUntilLocation(const std::string &searchString);
	void setServerName(void);
	void setPort(void);
	void setRoot(void);
	void setIndex(void);
	void setMethods(void);
	void setUpload(void);
	void setError(const std::string &errorPageNumber,
		const std::string &exceptionName);
	void setErrors(void);
	void printMethods(void);
	void setSockedFD();
	void setServer(int *epollFd, std::unordered_map<int, Server*> *connectedServers);
	char **makeEnv(int idx);
	int getServerFd(void);
	std::string getServerName(void);
	std::string getPortString(void);
	int getPort(void);
	std::string getRoot(void);
	std::string getIndex(void);
	std::string getUpload(void);
	int getSocketFD(void);
	Methods getMethods(void);
	socklen_t getAddrlen(void);
	std::string getMethodsList(void);
	struct sockaddr_in *getAddress(void);
	std::vector<Locations> getLocation(void);
	HttpHandler& getHttpHandler(int index);
	HttpHandler* matchSocketToHandler(const int& socket);
	int initSocketToHandler(const int &socket, char *buffer,  int bytes_rd);
	long long getClientBodySize(void);
	void setLocationsRegex(std::string serverContent);
	long long getFileSize(const std::string &filename, const int &idx);
	Server(std::string serverContent);
	void setSocketOptions(const int &opt);
	void initializeAddress();
	void bindAdressSocket();
	void listenToSocket();
	void makeResponse(const std::string &buffer, int index);
	void makeResponseForRedirect(int index);
	void readFile(int index);
	void setFileInServer(int idx);
	void setClientBodySize(void);
	void deleteFileInServer(int idx);
	std::string returnAutoIndex(const int &idx, std::string &uri);
	void clientConnectionFailed(int client_socket, int index);
	void sendResponse(const int &idx, int &socket);
	void sendFavIconResponse(const int &idx, int &socket);
	void linkHandlerResponseRequest(std::vector<request_t> &requests,
	std::vector<response_t> &responses, std::unordered_map<int, bool> *_socketReceivedFirstRequest);
	void checkFileDetails(const int &idx, std::ofstream &file);
	~Server();
};
