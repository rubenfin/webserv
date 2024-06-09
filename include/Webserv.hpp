/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jack <jack@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/09 14:51:39 by rfinneru          #+#    #+#             */
/*   Updated: 2024/06/09 22:30:49 by jack             ###   ########.fr       */
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

class Webserv
{
  protected:
	std::vector<Server> servers_;

  public:
	Webserv(std::string fileName);
	~Webserv();
};

Webserv::Webserv(std::string fileName)
{
	int	count;

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
	int index;
	std::string serverStrings[10];
	while (std::getline(file, line))
	{
		if (std::regex_search(line, startPattern))
		{
			// std::cout << "server switch" << std::endl;
			index++;
		}
		serverStrings[index] += line + '\n';
	}
	for (size_t i = 0; i <= index ; i++)
		servers_.emplace_back(serverStrings[i]);
}

Webserv::~Webserv()
{
}
// buffer << file.rdbuf();

// content = buffer.str();
// std::regex pattern(R"(server\s*\{)");
// std::sregex_iterator begin(content.begin(), content.end(), pattern);
// std::sregex_iterator end;
// for (std::sregex_iterator iter = begin; iter != end; ++iter)
// {
// 	std::smatch match = *iter;
// 	std::cout << match.str() << std::endl;