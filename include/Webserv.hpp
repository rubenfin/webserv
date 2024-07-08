/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Webserv.hpp                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: jade-haa <jade-haa@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/09 14:51:39 by rfinneru      #+#    #+#                 */
/*   Updated: 2024/07/05 13:58:34 by rfinneru      ########   odam.nl         */
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

// need to check this might be only for GET and not POST and DELETE
#define MAX_LENGTH_HTTP_REQ 8192
#define MAX_FDS 1024
#define MAX_EVENTS 16

class	Logger;
class	Server;
class	Locations;
class	HttpHandler;

class Webserv
{
  private:
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
	Webserv(std::string fileName, char **env);
	~Webserv();
};

size_t	ft_strlen(const char *s);
char	*ft_strjoin(char const *s1, char const *s2);
void	resetRequestResponse(request_t &request, response_t &response);
int		checkIfDir(const std::string &pathname);
int		checkIfFile(const std::string &pathname);
void	setNonBlocking(int *fd);
