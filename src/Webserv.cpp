/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Webserv.cpp                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: jade-haa <jade-haa@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/11 16:45:43 by rfinneru      #+#    #+#                 */
/*   Updated: 2024/08/21 13:41:23 by rfinneru      ########   odam.nl         */
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

Webserv::Webserv(std::string fileName, char **env)
{
	_epollFd = epoll_create(1);
	setConfig(fileName);
	_environmentVariables = env;
	interrupted = 0;
}

void Webserv::setEnv(char **env)
{
	_environmentVariables = env;
}

void Webserv::setupServers(socklen_t &addrlen)
{
	addrlen = sizeof(_servers[0].getAddress());
	_servers[0].setServer(_epollFd);
	logger.log(INFO, "Server " + _servers[0].getServerName()
		+ " started on port " + _servers[0].getPortString());
}

void Webserv::cleanHandlerRequestResponse()
{
	for (size_t i = 0; i < MAX_EVENTS; i++)
		_servers[0].getHttpHandler(i).cleanHttpHandler();
}

Webserv::~Webserv()
{
}