/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jade-haa <jade-haa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/09 14:51:39 by rfinneru          #+#    #+#             */
/*   Updated: 2024/06/11 16:07:58 by jade-haa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Server.hpp" // Include Server.hpp here
#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>
#include <vector>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

class Webserv
{
  protected:
	std::vector<Server> servers_;

  public:
	void printParsing(void)
	{
		std::cout << "Amount servers == " << servers_.size() << std::endl;

		for (size_t i = 0; i < servers_.size(); i++)
		{
			std::cout << i << "<---- server index" << std::endl;
			std::cout << "ServerName = " << servers_[i].getServerName() << std::endl;
			std::cout << "Port = " << servers_[i].getPort() << std::endl;
			std::cout << "Root = " << servers_[i].getRoot() << std::endl;
			std::cout << "Index = " << servers_[i].getIndex() << std::endl;
			std::cout << "Methods list = " << servers_[i].getMethodsList() << std::endl;
			std::vector<Locations> tmp = servers_[i].getLocation();
			for (size_t i = 0; i < tmp.size(); i++)
			{
				std::cout << "A location!! " << std::endl;
				tmp[i].printLocationsContent();
			}
			
		}
	}
Webserv(std::string fileName)
{
	int		count;
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
			if (serverStrings.size() <= index)
			{
				serverStrings.push_back("");
			}
			serverStrings[index] += line + '\n';
		}
	}
    if (!serverStrings.empty())
    {
        servers_.reserve(servers_.size() + serverStrings.size());
        for (const auto& serverString : serverStrings)
        {
            servers_.emplace_back(serverString);
        }
    }
}

~Webserv()
{
}
};

// buffer << file.rdbuf();

// content = buffer.str();
// std::regex pattern(R"(server\s*\{)");
// std::sregex_iterator begin(content.begin(), content.end(), pattern);
// std::sregex_iterator end;
// for (std::sregex_iterator iter = begin; iter != end; ++iter)
// {
// 	std::smatch match = *iter;
// 	std::cout << match.str() << std::endl;