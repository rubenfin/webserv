/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jade-haa <jade-haa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/09 14:51:39 by rfinneru          #+#    #+#             */
/*   Updated: 2024/08/10 14:14:17 by jade-haa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

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
#include <map>
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
#include <vector>

// need to check this might be only for GET and not POST and DELETE
#define BUFFERSIZE 4096
#define MAX_FDS 1024
#define MAX_EVENTS 16

extern volatile sig_atomic_t	interrupted;

class							Logger;
class							Server;
class							Locations;
class							HttpHandler;

extern Logger &logger;

class Webserv
{
  private:
	int _epollFd;
	epoll_event _event;
	std::string _response;
	std::vector<Server> _servers;
	char **_environmentVariables;

  public:
	void setupServers(socklen_t &addrlen);
	int execute(void);
	void printParsing(void);
	void setEnv(char **env);
	void setConfig(std::string fileName);
	void serverActions(const int&idx, int& socket);
	void readWriteServer(struct epoll_event *eventList, int idx, int client_socket, struct epoll_event eventConfig);
	void readFromSocketError(const int &err, const int &idx, int &socket);
	void readFromSocketSuccess(const int &idx, const char *buffer, const int& bytes_read);
	int acceptClientSocket(int &client_socket, socklen_t addrlen, int serverConnectionIndex, const int &i);
	void cleanHandlerRequestResponse();
	int checkForNewConnection(int eventFd);
	// void linkHandlerResponseRequest();
	void addFdToReadEpoll(epoll_event &eventConfig, int &client_socket);
	void removeFdFromEpoll(int &socket);
	void setFdReadyForRead(epoll_event &eventConfig, int &socket);
	void setFdReadyForWrite(epoll_event &eventConfig, int &client_tmp);
	Webserv(std::string fileName, char **env);
	~Webserv();
};
