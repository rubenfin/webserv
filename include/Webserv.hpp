/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jade-haa <jade-haa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/09 14:51:39 by rfinneru          #+#    #+#             */
/*   Updated: 2024/09/04 15:40:51 by jade-haa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "CGI.hpp"
#include "Exceptions.hpp"
#include "HttpHandler.hpp"
#include "Logger.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "Server.hpp"
#include "Utils.hpp"
#include <dirent.h>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <netinet/in.h>
#include <regex>
#include <signal.h>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <unordered_map>
#include <vector>

// need to check this might be only for GET and not POST and DELETE
#define BUFFERSIZE 1024
#define MAX_FDS 1024
#define MAX_EVENTS 64
#define MIN_SIZE 5

extern volatile sig_atomic_t	interrupted;

class							Logger;
class							Server;
class							Locations;
class							HttpHandler;

extern Logger &logger;

/*
	reinterpret cast to get the diffence between the stored pointes in the unordered map
	this determines if it's a Server, Client or maybe even CGI.
	each Client has a single HTTP (request/response) Handler.
*/

class Webserv
{
  protected:
	int _epollFd;
	epoll_event _event;
	HttpHandler *_handler;
	std::vector<Server> _servers;
	std::unordered_map<int, Server *> _socketsConnectedToServers;

  public:
	void addSocketToServer(const int& socket, Server* server, char buffer[BUFFERSIZE]);
	std::unordered_map<int, Server *> &getSocketsConnectedToServers(void);
	void setupServers(socklen_t &addrlen);
	int execute(void);
	void checkCGItimeouts(void);
	void printParsing(void);
	Server *findServerConnectedToSocket(const int &socket);
	void addFdToReadEpoll(epoll_event &eventConfig, int &socket);
	void addFdToWriteEpollWB(epoll_event &eventConfig, int &socket);
	int checkForNewConnection(int eventFd);
	int acceptClientSocket(int &client_socket, socklen_t addrlen, const int &i,
		int server);
	void cleanHandlerRequestResponse();
	void setConfig(std::string fileName);
	Webserv(std::string fileName);
	~Webserv();
};
