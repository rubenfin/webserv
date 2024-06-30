/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Server.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: jade-haa <jade-haa@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/09 15:40:25 by jade-haa      #+#    #+#                 */
/*   Updated: 2024/06/30 14:48:57 by rfinneru      ########   odam.nl         */
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

struct					Methods
{
	bool				GET;
	bool				POST;
	bool				PUT;
	bool				DELETE;
	bool				PATCH;
} typedef Methods;

class					Locations;

class					HttpHandler;

class Server
{
  protected:
	std::string _error404;
	std::string _serverContent;
	std::string _portString;
	std::string _methodsList;
	char				*_buffer;
	char				*_response;
	int					_socketfd;
	std::string 		_serverName;
	int					_port;
	std::string 		_root;
	std::string 		_index;
	Methods				_allowedMethods;
	std::vector<Locations> _locations;

	struct sockaddr_in	*_address;
	socklen_t			_addrlen;

	HttpHandler			*_http_handler;

  public:
	char *cgi(char **env);
	void execute_CGI_script(pid_t pid, int *fds, const char *script,
		char **env);
	void getLocationStack(std::string locationContent);
	std::string extractValue(const std::string &searchString);
	void setServerName(void);
	void setPort(void);
	void setRoot(void);
	void setIndex(void);
	void setMethods(void);
	void setError404(void);
	void printMethods(void);
	void setSockedFD(int fd);
	void setServer();
	void setEnv(char **&env);
	char *getResponse(void);
	std::string getServerName(void);
	std::string getPortString(void);
	u_int16_t getPort(void);
	std::string getRoot(void);
	std::string getIndex(void);
	int getSocketFD(void);
	Methods getMethods(void);
	socklen_t getAddrlen(void);
	std::string getMethodsList(void);
	struct sockaddr_in *getAddress(void);
	std::vector<Locations> getLocation(void);
	HttpHandler *getHttpHandler(void);
	std::string getError404(void);
	void setLocationsRegex(std::string serverContent);
	Server(std::string serverContent);
	void makeResponse(char *buffer);
	void readFile(void);
	~Server();
};
