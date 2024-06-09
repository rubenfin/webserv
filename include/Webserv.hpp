/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jade-haa <jade-haa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/09 14:51:39 by rfinneru          #+#    #+#             */
/*   Updated: 2024/06/09 17:56:39 by jade-haa         ###   ########.fr       */
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
	buffer << file.rdbuf();
	content = buffer.str();
	std::regex pattern(R"(server\s*\{([^}]*)\})");
	std::sregex_iterator begin(content.begin(), content.end(), pattern);
	std::sregex_iterator end;
	for (std::sregex_iterator iter = begin; iter != end; ++iter)
	{
        Server[
		std::smatch match = *iter;
		// Store the captured group (the content inside the braces)
		serverConfigs.push_back(match[1].str());
	}
}

Webserv::~Webserv()
{
}