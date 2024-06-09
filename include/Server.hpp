/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jack <jack@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/09 15:40:25 by jade-haa          #+#    #+#             */
/*   Updated: 2024/06/09 22:31:00 by jack             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Locations.hpp"
#include <iostream>
#include <regex>
#include <sstream>
#include <string>
#include <unordered_set>

enum class HttpMethod
{
	GET,
	POST,
	PUT,
	DELETE,
	PATCH
};

class Server
{
  protected:
	std::string serverContent_;
	std::string serverName_;
	std::string port_;
	std::string root_;
	std::string index_;
	std::unordered_set<HttpMethod> allowMethods_;
	std::vector<Locations> locations_;

  public:
	std::string extractValue(const std::string &searchString)
	{
		size_t pos = serverContent_.find(searchString);
		if (pos != std::string::npos)
		{
			pos += searchString.length();
			size_t endPos = serverContent_.find('\n', pos);
			if (endPos != std::string::npos)
				return (serverContent_.substr(pos, endPos - pos));
			else
				return (serverContent_.substr(pos));
		}
		return ("");
	}
	void setServerName(std::string serverContent)
	{
		serverName_ = extractValue("server_name");
		// std::cout << "serverName " << serverName_ << std::endl;
	}
	void setPort(std::string serverContent)
	{
		port_ = extractValue("listen");
		// std::cout << "port " << port_ << std::endl;
	}
	void setRoot(std::string serverContent)
	{
		root_ = extractValue("root");
		// std::cout << "root " << root_ << std::endl;
	}
	void setIndex(std::string serverContent)
	{
		index_ = extractValue("index");
		// std::cout << "index " << index_ << std::endl;
	}
	void setMethods(std::string serverContent)
	{
		std::string methodsList = extractValue("methods");
		// std::cout << "methods " << methodsList << std::endl;
	}
	void setLocations(std::string serverContent)
	{
		int index = 0;
		bool copyAllowed = false;
		std::string line;
		std::istringstream iss(serverContent);
		std::regex startPattern(R"(location)");
		std::string locationsStrings[10];

		while (std::getline(iss, line))
		{
			if (std::regex_search(line, startPattern))
			{
				copyAllowed = true;
				index++;
			}
			if (copyAllowed)
				locationsStrings[index] += line + '\n';
		}
		for (size_t i = 0; i <= index; i++)
			locations_.emplace_back(locationsStrings[i]);

    	std::cout << "New server" << std::endl;

	}

	Server(std::string serverContent)
	{
		serverContent_ = serverContent;
		setServerName(serverContent);
		setPort(serverContent);
		setRoot(serverContent);
		setIndex(serverContent);
		setMethods(serverContent);
		setLocations(serverContent);
	}
};