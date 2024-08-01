/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Webserv.hpp                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: jade-haa <jade-haa@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/09 14:51:39 by rfinneru      #+#    #+#                 */
/*   Updated: 2024/08/01 16:03:46 by rfinneru      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Exceptions.hpp"
#include "HttpHandler.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "Server.hpp"
#include "Utils.hpp"
#include <fstream>
#include <iostream>
#include <map>
#include <netinet/in.h>
#include <regex>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>
#include <sys/epoll.h>
#include <signal.h>
#include "Logger.hpp"
#include <dirent.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

// need to check this might be only for GET and not POST and DELETE
#define BUFFERSIZE 1024
#define MAX_FDS 1024
#define MAX_EVENTS 16

extern volatile sig_atomic_t	interrupted;


class	Logger;
class	Server;
class	Locations;
class	HttpHandler;

extern Logger &logger;

class Webserv
{
  private:
	int _epollFd;
	epoll_event _event;
	std::string _response;
	HttpHandler *_handler;
	std::vector<Server> _servers;
	char **_environmentVariables;

  public:
	int execute(void);
	void printParsing(void);
	void setEnv(char **env);
	void setConfig(std::string fileName);
	void serverActions(int client_socket, request_t &request, response_t &response, int index);
	int acceptClienSocket(int& client_socket, socklen_t addrlen, const int &i);
	Webserv(std::string fileName, char **env);
	~Webserv();
};
