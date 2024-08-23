/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Webserv.cpp                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: jade-haa <jade-haa@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/11 16:45:43 by rfinneru      #+#    #+#                 */
/*   Updated: 2024/08/23 12:30:44 by rfinneru      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Webserv.hpp"

void Webserv::printParsing(void)
{
	std::cout << "Amount servers == " << _servers.size() << std::endl;
	for (size_t i = 0; i < _servers.size(); i++)
	{
		// std::cout << i << "<---- server index" << std::endl;
		// std::cout << "ServerName = " << _servers[i].getServerName() << std::endl;
		// std::cout << "Port = " << _servers[i].getPort() << std::endl;
		// std::cout << "Root = " << _servers[i].getRoot() << std::endl;
		// std::cout << "Index = " << _servers[i].getIndex() << std::endl;
		// std::cout << "Methods list = " << _servers[i].getMethodsList() << std::endl;
		// _servers[i].printMethods();
		std::vector<Locations> tmp = _servers[i].getLocation();
		for (size_t i = 0; i < tmp.size(); i++)
		{
			std::cout << "Location -->" << i << std::endl;
			// tmp[i].printLocationsContent();
			std::cout << tmp[i].getLocationDirective() << std::endl;
			tmp[i].printMethods();
			std::cout << "Root " << tmp[i].getRoot() << std::endl;
			std::cout << "Index " << tmp[i].getIndex() << std::endl;
			std::cout << "Cgi_pass " << tmp[i].getCgi_pass() << std::endl;
		}
	}
}
void Webserv::setConfig(std::string fileName)
{
	int		index;
	bool	flag;

	index = 0;
	flag = false;
	std::string content;
	std::stringstream buffer;
	std::ifstream file(fileName);
	if (!file.is_open())
	{
		std::cerr << "Error: Unable to open file " << fileName << std::endl;
		return ;
	}
	std::string line;
	std::regex startPattern(R"(server\s*\{)");
	std::vector<std::string> serverStrings;
	while (std::getline(file, line))
	{
		if (std::regex_search(line, startPattern))
		{
			if (flag)
			{
				serverStrings.push_back("");
				index++;
			}
			flag = true;
		}
		if (flag)
		{
			if ((int)serverStrings.size() <= index)
			{
				serverStrings.push_back("");
			}
			serverStrings[index] += line + '\n';
		}
	}
	if (!serverStrings.empty())
	{
		_servers.reserve(_servers.size() + serverStrings.size());
		for (const auto &serverString : serverStrings)
		{
			_servers.emplace_back(serverString);
		}
	}
}

int Webserv::checkForNewConnection(int eventFd)
{
	
	for (size_t i = 0; i < _servers.size(); i++)
	{
		if (eventFd == _servers[i].getServerFd())
			return i;
	}
	return -1;
}

Webserv::Webserv(std::string fileName)
{
	_epollFd = epoll_create(1);
	setConfig(fileName);
	interrupted = 0;
	_socketsConnectedToServers.reserve(MAX_EVENTS);
}


void Webserv::setupServers(socklen_t &addrlen)
{
	std::cout << "amount of servers = " << _servers.size() << std::endl;
	for (size_t i = 0; i < _servers.size(); i++)
	{
		addrlen = sizeof(_servers[i].getAddress());
		_servers[i].setServer(&_epollFd, &_socketsConnectedToServers);
		logger.log(INFO, "Server " + _servers[i].getServerName()
			+ " started on port " + _servers[i].getPortString());
	}
}

void Webserv::cleanHandlerRequestResponse()
{
	for (size_t i = 0; i < _servers.size(); i++)
	{
		for (size_t j = 0; j < MAX_EVENTS; j++)
			_servers[i].getHttpHandler(j).cleanHttpHandler();

	}
	
}

std::unordered_map<int, Server*> Webserv::getSocketsConnectedToServers(void)
{
	return (_socketsConnectedToServers);
}

void Webserv::addSocketToServer(const int& socket, Server* server)
{
	std::cout << "here" << std::endl;
	std::cout << socket << "|" << server << std::endl;
	auto result = _socketsConnectedToServers.insert({socket, server});
	
	if (!result.second)
	{
		logger.log(ERR, "Couldn't add socket to servers connected");
		return ;
	}
	logger.log(INFO, "Paired socket: " + std::to_string(socket) + " to Server with FD " + std::to_string(server->getServerFd()));
}

Webserv::~Webserv()
{
}