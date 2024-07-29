/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jade-haa <jade-haa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/09 14:51:39 by rfinneru          #+#    #+#             */
/*   Updated: 2024/07/29 14:29:47 by jade-haa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Exceptions.hpp"
#include "HttpHandler.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "Server.hpp"
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
	void setResponse(const std::string &filePath);
	void setEnv(char **env);
	void setConfig(std::string fileName);
	void serverActions(int client_socket, request_t request, response_t response);
	Webserv(std::string fileName, char **env);
	~Webserv();
};

size_t	ft_strlen(const char *s);
char	*ft_strjoin(char const *s1, char const *s2);
void	resetRequestResponse(request_t &request, response_t &response);
int		checkIfDir(const std::string &pathname);
int		checkIfFile(const std::string &pathname);
void	setNonBlocking(int *fd);
bool	hasSpecialCharacters(const std::string& fileName);
void 	replaceEncodedSlash(std::string &str);