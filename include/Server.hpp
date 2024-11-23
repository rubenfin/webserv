/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Server.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: jade-haa <jade-haa@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/09 15:40:25 by jade-haa      #+#    #+#                 */
/*   Updated: 2024/11/11 12:06:55 by rfinneru      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "CGI.hpp"
#include "Exceptions.hpp"
#include "HTTPHandler.hpp"
#include "Locations.hpp"
#include "Webserv.hpp"
#include <fcntl.h>
#include <iostream>
#include <netinet/in.h>
#include <regex>
#include <sstream>
#include <string>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <cstring>

#define MAX_EVENTS 64

class	Locations;
class	HTTPHandler;
class	HttpException;
struct CGI_t;

struct Methods {
  bool GET;
  bool POST;
  bool DELETE;
} typedef Methods;

class Server {
protected:
  std::unordered_map<int, Server *> *_connectedServersPtr;
  std::map<int, CGI_t *> _fdsRunningCGI;
  int *_epollFDptr;
  std::string _serverContent;
  std::string _host;
  std::string _portString;
  std::string _methodsList;
  int _serverFd;
  std::string _serverName;
  std::string _serverNameIP;
  int _port;
  std::string _root;
  std::string _index;
  std::string _upload;
  Methods _allowedMethods;
  std::vector<Locations> _locations;
  struct sockaddr_in *_address;
  socklen_t _addrlen;
  long long _client_body_size_server;
  std::vector<HTTPHandler> _http_handler;
  

public:
  std::vector<HTTPHandler>& getHTTPHandlers(void);
  int readFromFile(const int &fd, HTTPHandler &handler);
  void readWriteCGI(const int& socket, HTTPHandler &handler);
  void removeCGIrunning(const int& socket);
  std::map<int, CGI_t *> &getFdsRunningCGI(void);
  void removeSocketAndServer(const int& socket);
  void addFdToReadEpoll(epoll_event &eventConfig, int &socket);
  void readFromSocketError(const int &err, HTTPHandler &handler, int &socket);
  int serverActions(HTTPHandler &handler, int &socket);
  void readFromSocketSuccess(HTTPHandler &handler, const char *buffer,
                             const int &bytes_read);
  void removeFdFromEpoll(const int &socket);
  void setFdReadyForRead(const int &socket);
  void setFdReadyForWrite(const int &socket);
  void readWriteServer(epoll_event &event, HTTPHandler &handler);
  void cgi(HTTPHandler &handler, const int& socket);
  void execute_CGI_script(int *writeSide, int *readSide, const char *script,
                          HTTPHandler &handler);
  void getLocationStack(std::string locationContent);
  void logThrowStatus(HTTPHandler &handler, const level &lvl, const std::string &msg,
                      const httpStatusCode &status, HttpException exception);
  std::string extractValue(const std::string &searchString);
  std::string extractValueUntilLocation(const std::string &searchString);
  void checkIfBin(HTTPHandler &handler);
  void setServerName(void);
  void setPortHost(void);
  void setRoot(void);
  void setIndex(void);
  void setMethods(void);
  void setUpload(void);
  void setError(const std::string &errorPageNumber,
                const std::string &exceptionName);
  void setErrors(void);
  void printMethods(void);
  void setSockedFD();
  void setServer(int *epollFd,
                 std::unordered_map<int, Server *> *connectedServers);
  char **makeEnv(HTTPHandler &handler);
  int getServerFd(void);
  std::string getServerName(void);
  std::string getPortString(void);
  std::string getHost(void);
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
  HTTPHandler &getHTTPHandler(const int& idx);
  HTTPHandler *matchSocketToHandler(const int &socket);
  int initSocketToHandler(const int &socket, char *buffer, int bytes_rd);
  long long getClientBodySize(void);
  void setLocationsRegex(std::string serverContent);
  long long getFileSize(const std::string &filename, HTTPHandler &handler);
  Server(std::string serverContent);
  void setSocketOptions(const int &opt);
  void initializeAddress();
  void bindAdressSocket();
  void listenToSocket();
  void makeResponse(const std::string &buffer, HTTPHandler &handler);
  void makeResponseForRedirect(HTTPHandler &handler);
  void readFile(HTTPHandler &handler);
  void setFileInServer(HTTPHandler &handler);
  void setClientBodySize(void);
  void deleteFileInServer(HTTPHandler &handler);
  std::string returnAutoIndex(HTTPHandler &handler, std::string &uri);
  void clientConnectionFailed(int client_socket, HTTPHandler &handler);
  void sendResponse(HTTPHandler &handler, const int &socket);
  void sendFavIconResponse(HTTPHandler &handler, const int &socket);
  void linkHandlerResponseRequest(std::unordered_map<int, bool> *_socketReceivedFirstRequest);
  void checkFileDetails(HTTPHandler &handler, std::ofstream &file);
  ~Server();
};
