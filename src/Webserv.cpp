/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Webserv.cpp                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: jade-haa <jade-haa@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/11 16:45:43 by rfinneru      #+#    #+#                 */
/*   Updated: 2024/06/21 14:05:33 by rfinneru      ########   odam.nl         */
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
Webserv::Webserv(std::string fileName)
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
	setResponse("files/response_files/test.html");
}

void Webserv::setEnv(char **env)
{
	_environmentVariables = env;
}

void Webserv::setResponse(const std::string &filePath)
{
	std::ifstream file(filePath);
	if (!file.is_open())
	{
		std::cerr << "Error: Unable to open file " << filePath << std::endl;
		return;
	}
	std::stringstream buffer;
	buffer << file.rdbuf();
	std::string response = buffer.str();
	_response = "HTTP/1.1 200 OK\nContent-type: text/html\n\n" + response;
}
Webserv::~Webserv()
{
}