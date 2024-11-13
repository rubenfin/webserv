/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Webserv.hpp                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: jade-haa <jade-haa@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/09 14:51:39 by rfinneru      #+#    #+#                 */
/*   Updated: 2024/11/13 16:24:29 by rfinneru      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "CGI.hpp"
#include "Exceptions.hpp"
#include "HTTPHandler.hpp"
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
#include <filesystem>
#include <sstream>

#define BUFFERSIZE 8192
// #define MAX_FDS 1024
#define MAX_EVENTS 64
#define MIN_SIZE 5

extern volatile sig_atomic_t	interrupted;

class							Logger;
class							Server;
class							Locations;
class							HTTPHandler;

extern Logger &logger;

class Webserv
{
  protected:
	int _epollFd;
	epoll_event _event;
	HTTPHandler *_handler;
	std::vector<Server> _servers;
	std::unordered_map<int, Server *> _socketsConnectedToServers;
	std::unordered_map<int, bool> _socketReceivedFirstRequest;

  public:
	Webserv(char *fileName);
	~Webserv();

	void initalizeServers(socklen_t &addrlen);
	void cleanUpServers();
	int handleEvent(epoll_event *eventList, const int &event_fd, int idx);
	int initializeConnection(const socklen_t &addrlen, int &client_socket,
		const int &serverConnectIndex);
	void removeFdFromEpoll(const int &socket);
	void insertSocketIntoReceivedFirstRequest(const int &socket);
	int handleFirstRequest(const int &client_socket);
	int findRightServer(const std::string &buffer);
	void removeSocketFromReceivedFirstRequest(const int &socket);
	void addSocketToServer(const int &socket, Server *server);
	std::unordered_map<int, Server *> &getSocketsConnectedToServers(void);
	void setupServers(socklen_t &addrlen);
	int execute(void);
	void checkCGItimeouts(void);
	void printParsing(void);
	Server *findServerConnectedToSocket(const int &socket);
	bool getServerReceivedFirstRequest(const int &socket);
	void addFdToReadEpoll(int &socket);
	int checkForNewConnection(int eventFd);
	int acceptClientSocket(int &client_socket, socklen_t addrlen,
		const int &server);
	void cleanHandlerRequestResponse();
	void setConfig(std::string fileName);
};
