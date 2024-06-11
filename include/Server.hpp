/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Server.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: jade-haa <jade-haa@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/09 15:40:25 by jade-haa      #+#    #+#                 */
/*   Updated: 2024/06/11 16:39:41 by rfinneru      ########   odam.nl         */
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
	int socket;
	std::string serverContent_;
	std::string serverName_;
	std::string portString_;
	u_int16_t port_;
	std::string root_;
	std::string index_;
	std::string methodsList_;
	std::unordered_set<HttpMethod> allowMethods_;
	std::vector<Locations> locations_;

  public:
	void getLocationStack(std::string locationContent)
	{
		std::vector<std::string> result;
		std::string line;
		std::stack<char> bracketStack;
		bool insideBrackets = false;
		std::ostringstream currentContent;
		std::istringstream iss(locationContent);

		while (std::getline(iss, line))
		{
			for (char ch : line)
			{
				if (ch == '{')
				{
					bracketStack.push(ch);
					if (bracketStack.size() == 1)
					{
						insideBrackets = true;
						currentContent.str("");
					}
				}
				else if (ch == '}')
				{
					if (!bracketStack.empty())
					{
						bracketStack.pop();
						if (bracketStack.empty())
						{
							insideBrackets = false;
							result.push_back(currentContent.str());
						}
					}
				}
				if (insideBrackets)
				{
					currentContent << ch;
				}
			}
			if (insideBrackets)
			{
				currentContent << '\n';
			}
		}
		for (const auto &line : result)
		{
			locations_.emplace_back(line);
			// std::cout << "Location" << line << std::endl;
		}
	}
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
		portString_ = extractValue("listen");
		port_ = std::stoi(portString_);
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
		methodsList_ = extractValue("methods");
		// std::cout << "methods " << methodsList << std::endl;
	}

	std::string getServerName(void)
	{
		return (serverName_);
	}
	std::string getPortString(void)
	{
		return (portString_);
	}

	u_int16_t getPort(void)
	{
		return (port_);
	}

	std::string getRoot(void)
	{
		return (root_);
	}
	std::string getIndex(void)
	{
		return (index_);
	}
	std::unordered_set<HttpMethod> getMethods(void)
	{
		return (allowMethods_);
	}

	std::string getMethodsList(void)
	{
		return (methodsList_);
	}

	std::vector<Locations> getLocation(void)
	{
		return (locations_);
	}

	void setLocationsRegex(std::string serverContent)
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
		{
			// std::cout << "Regex parsing set Locations\n" << locationsStrings[i];
			locations_.emplace_back(locationsStrings[i]);
		}

		// std::cout << "New server" << std::endl;
	}

	Server(std::string serverContent)
	{
		serverContent_ = serverContent;
		// std::cout << "test0" << std::endl;
		std::cout << serverContent << std::endl;
		setServerName(serverContent);
		// std::cout << "test1" << std::endl;
		setPort(serverContent);
		// std::cout << "test2" << std::endl;

		setRoot(serverContent);
		// std::cout << "test3" << std::endl;

		setIndex(serverContent);
		// std::cout << "test4" << std::endl;

		setMethods(serverContent);
		// std::cout << "test5" << std::endl;

		setLocationsRegex(serverContent);
		// std::cout << "test6" << std::endl;

		// getLocationStack(serverContent);
	}
};