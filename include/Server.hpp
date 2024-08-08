/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Server.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: jade-haa <jade-haa@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/09 15:40:25 by jade-haa      #+#    #+#                 */
/*   Updated: 2024/08/08 15:52:33 by rfinneru      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "HttpHandler.hpp"
#include "Locations.hpp"
#include "Webserv.hpp"
#include <fcntl.h>
#include <iostream>
#include <netinet/in.h>
#include <regex>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <unordered_set>

#define MAX_EVENTS 16

class					Locations;
class					HttpHandler;

struct					Methods
{
	bool				GET;
	bool				POST;
	bool				DELETE;
} typedef Methods;

class Server
{
  protected:
	std::string 		_error404;
	std::string 		_serverContent;
	std::string 		_portString;
	std::string 		_methodsList;
	char				*_buffer;
	int					_serverFd;
	std::string 		_serverName;
	int					_port;
	std::string 		_root;
	std::string 		_index;
	std::string			_upload;
	Methods				_allowedMethods;
	std::vector<Locations> _locations;
	struct sockaddr_in	*_address;
	socklen_t			_addrlen;
	HttpHandler			*_http_handler[MAX_EVENTS];

  public:
	void cgi(char **env, int index);
	void execute_CGI_script(int *fds, const char *script, char **env, int index);
	void getLocationStack(std::string locationContent);
	std::string extractValue(const std::string &searchString);
	std::string extractValueUntilLocation(const std::string &searchString);
	void setServerName(void);
	void setPort(void);
	void setRoot(void);
	void setIndex(void);
	void setMethods(void);
	void setUpload(void);
	void setError404(void);
	void printMethods(void);
	void setSockedFD();
	void setServer(int epollFd);
	void setEnv(char **&env, int index);
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
	HttpHandler *getHttpHandler(int index);
	std::string getError404(void);
	void setLocationsRegex(std::string serverContent);
	Server(std::string serverContent);
	void setSocketOptions(const int &opt);
	void initializeAddress();
	void bindAdressSocket();
	void listenToSocket();
	void makeResponse(char *buffer, int index);
	void makeResponseForRedirect(int index);
	void readFile(int index);
	void setFileInServer(int idx);
	void deleteFileInServer(int idx);
	std::string returnAutoIndex(std::string &uri);
	void clientConnectionFailed(int client_socket, int index);
	void sendResponse(const int &idx, int &socket);
	void sendFavIconResponse(const int &idx, int &socket);
	void sendNotFoundResponse(const int& idx, int &socket);
	void linkHandlerResponseRequest(request_t *request, response_t *response);
	void checkFileDetails(const int &idx, std::ofstream &file);
	~Server();
};
