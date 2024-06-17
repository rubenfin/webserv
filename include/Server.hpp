/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Server.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: jade-haa <jade-haa@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/09 15:40:25 by jade-haa      #+#    #+#                 */
/*   Updated: 2024/06/17 15:46:19 by rfinneru      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "HttpHandler.hpp"
#include "Locations.hpp"
#include "Webserv.hpp"
#include <iostream>
#include <netinet/in.h>
#include <regex>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <unordered_set>
#include <fcntl.h>

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
	int					_socketfd;
	std::string _serverContent;
	std::string _portString;
	std::string _methodsList;
	char * _stringFromFile;

	std::string _serverName;
	u_int16_t			_port;
	std::string _root;
	std::string _index;
	Methods				_allowedMethods;
	std::vector<Locations> _locations;

	struct sockaddr_in	*_address;
	socklen_t			_addrlen;

	HttpHandler			*_http_handler;

  public:
	void getLocationStack(std::string locationContent);
	std::string extractValue(const std::string &searchString);
	void setServerName(void);
	void setPort(void);
	void setRoot(void);
	void setIndex(void);
	void setMethods(void);
	void printMethods(void);
	void setSockedFD(int fd);
	void setServer();
	char * getStringFromFile(void);
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
	void setLocationsRegex(std::string serverContent);
	Server(std::string serverContent);
	Locations *findMatchingDirective(void);
	void readFile(void);
	~Server();
};
