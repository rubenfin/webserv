/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Server.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: rfinneru <rfinneru@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/11 17:00:53 by rfinneru      #+#    #+#                 */
/*   Updated: 2024/06/11 17:14:04 by rfinneru      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"

void Server::getLocationStack(std::string locationContent)
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
			_locations.emplace_back(line);
			// std::cout << "Location" << line << std::endl;
		}
	}
	std::string Server::extractValue(const std::string &searchString)
	{
		size_t pos = _serverContent.find(searchString);
		if (pos != std::string::npos)
		{
			pos += searchString.length();
			size_t endPos = _serverContent.find('\n', pos);
			if (endPos != std::string::npos)
				return (_serverContent.substr(pos, endPos - pos));
			else
				return (_serverContent.substr(pos));
		}
		return ("");
	}
	void Server::setServerName(void)
	{
		_serverName = extractValue("server_name");
		// std::cout << "serverName " << _serverName << std::endl;
	}
	void Server::setPort(void)
	{
		_portString = extractValue("listen");
		_port = std::stoi(_portString);
		// std::cout << "port " << _port << std::endl;
	}
	void Server::setRoot(void)
	{
		_root = extractValue("root");
		// std::cout << "root " << _root << std::endl;
	}
	void Server::setIndex(void)
	{
		_index = extractValue("index");
		// std::cout << "index " << _index << std::endl;
	}
	void Server::setMethods(void)
	{
		_methodsList = extractValue("methods");
		// std::cout << "methods " << methodsList << std::endl;
	}

	std::string Server::getServerName(void)
	{
		return (_serverName);
	}
	std::string Server::getPortString(void)
	{
		return (_portString);
	}

	u_int16_t Server::getPort(void)
	{
		return (_port);
	}

	std::string Server::getRoot(void)
	{
		return (_root);
	}
	std::string Server::getIndex(void)
	{
		return (_index);
	}
	std::unordered_set<HttpMethod> Server::getMethods(void)
	{
		return (_allowMethods);
	}

	std::string Server::getMethodsList(void)
	{
		return (_methodsList);
	}

	std::vector<Locations> Server::getLocation(void)
	{
		return (_locations);
	}

	void Server::setLocationsRegex(std::string serverContent)
	{
		size_t index = 0;
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
			_locations.emplace_back(locationsStrings[i]);
		}

		// std::cout << "New server" << std::endl;
	}

	Server::Server(std::string serverContent)
	{
		_serverContent = serverContent;
		// std::cout << "test0" << std::endl;
		std::cout << serverContent << std::endl;
		setServerName();
		// std::cout << "test1" << std::endl;
		setPort();
		// std::cout << "test2" << std::endl;

		setRoot();
		// std::cout << "test3" << std::endl;

		setIndex();
		// std::cout << "test4" << std::endl;

		setMethods();
		// std::cout << "test5" << std::endl;

		setLocationsRegex(serverContent);
		// std::cout << "test6" << std::endl;

		// getLocationStack();
	}