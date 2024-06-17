/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Server.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: jade-haa <jade-haa@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/11 17:00:53 by rfinneru      #+#    #+#                 */
/*   Updated: 2024/06/17 16:01:12 by rfinneru      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"

void Server::setServer()
{
	int	opt;

	opt = 1;
	this->_address = new struct sockaddr_in;
	_addrlen = sizeof(*this->_address);
	// Correctly set the size based on the allocated structure
	setSockedFD(socket(AF_INET, SOCK_STREAM, 0));
	if (getSocketFD() < 0)
	{
		perror("socket failed");
		delete this->_address; // Clean up allocated memory
		exit(EXIT_FAILURE);
	}
	if (setsockopt(getSocketFD(), SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt,
			sizeof(opt)))
	{
		perror("setsockopt");
		delete this->_address; // Clean up allocated memory
		exit(EXIT_FAILURE);
	}
	this->_address->sin_family = AF_INET;
	this->_address->sin_addr.s_addr = INADDR_ANY;
	this->_address->sin_port = htons(getPort());
	if (bind(getSocketFD(), (struct sockaddr *)_address, sizeof(*_address)) < 0)
	{
		perror("bind failed");
		delete _address; // Clean up allocated memory
		exit(EXIT_FAILURE);
	}
	if (listen(getSocketFD(), 5) < 0)
	{
		perror("listen");
		delete _address; // Clean up allocated memory
		exit(EXIT_FAILURE);
	}
	_http_handler = new HttpHandler;
}

void Server::getLocationStack(std::string locationContent)
{
	bool	insideBrackets;

	std::vector<std::string> result;
	std::string line;
	std::stack<char> bracketStack;
	insideBrackets = false;
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
	size_t	pos;
	size_t	endPos;

	pos = _serverContent.find(searchString);
	if (pos != std::string::npos)
	{
		pos += searchString.length();
		endPos = _serverContent.find('\n', pos);
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
	_root.erase(remove_if(_root.begin(), _root.end(), isspace), _root.end());
	// std::cout << "root " << _root << std::endl;
}
void Server::setIndex(void)
{
	_index = extractValue("index");
	_index.erase(remove_if(_index.begin(), _index.end(), isspace), _index.end());
	// std::cout << "index " << _index << std::endl;
}

void Server::setMethods(void)
{
	_methodsList = extractValue("methods");
	_allowedMethods.GET = false;
	_allowedMethods.POST = false;
	_allowedMethods.DELETE = false;
	if (_methodsList.find("GET") != std::string::npos)
		_allowedMethods.GET = true;
	if (_methodsList.find("POST") != std::string::npos)
		_allowedMethods.POST = true;
	if (_methodsList.find("DELETE") != std::string::npos)
		_allowedMethods.DELETE = true;
	// std::cout << "methods " << methodsList << std::endl;
}

void Server::printMethods(void)
{
	std::cout << "GET Methods == " << _allowedMethods.GET << std::endl;
	std::cout << "POST Methods == " << _allowedMethods.POST << std::endl;
	std::cout << "DELETE Methods == " << _allowedMethods.DELETE << std::endl;
}

void Server::setSockedFD(int fd)
{
	this->_socketfd = fd;
}

int Server::getSocketFD(void)
{
	return (this->_socketfd);
}

char * Server::getStringFromFile(void)
{
	return (_stringFromFile);
}


std::string Server::getServerName(void)
{
	return (_serverName);
}
std::string Server::getPortString(void)
{
	return (_portString);
}

socklen_t Server::getAddrlen(void)
{
	return (_addrlen);
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
Methods Server::getMethods(void)
{
	return (_allowedMethods);
}

std::string Server::getMethodsList(void)
{
	return (_methodsList);
}

std::vector<Locations> Server::getLocation(void)
{
	return (_locations);
}

HttpHandler *Server::getHttpHandler(void)
{
	return(_http_handler);
}

struct sockaddr_in *Server::getAddress(void)
{
	return (_address);
}

void Server::setLocationsRegex(std::string serverContent)
{
	size_t	index;
	bool	copyAllowed;
	int		count;

	index = 0;
	copyAllowed = false;
	std::string line;
	std::istringstream iss(serverContent);
	std::regex startPattern(R"(location)");
	std::string locationsStrings[10];
	while (std::getline(iss, line))
	{
		if (std::regex_search(line, startPattern))
		{
			if (copyAllowed)
				index++;
			copyAllowed = true;
		}
		if (copyAllowed)
			locationsStrings[index] += line + '\n';
	}
	count = 0;
	for (size_t i = 0; i <= index; i++)
	{
		count++;
		_locations.emplace_back(locationsStrings[i]);
	}
}

Server::Server(std::string serverContent)
{
	_serverContent = serverContent;
	// std::cout << "test0" << std::endl;
	// std::cout << serverContent << std::endl;
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

void Server::readFile(void)
{
	int file;
	_stringFromFile = (char * )malloc(100000 * sizeof(char));
	std::cout << "response url " << _http_handler->getResponseURL() << std::endl;
	file = open(_http_handler->getResponseURL().c_str(), O_RDONLY);
	if (file == -1)
	{
		perror("opening file of responseURL");
		return;
	}
	int rdbytes = read(file, _stringFromFile, 100000);
	_stringFromFile[rdbytes] = '\0';
	close(file);
	std::cout << "file In String: " << _stringFromFile << std::endl;
	_stringFromFile = ft_strjoin("HTTP/1.1 200 OK\n\n",_stringFromFile);
}

Server::~Server()
{
	delete	_http_handler;
}
