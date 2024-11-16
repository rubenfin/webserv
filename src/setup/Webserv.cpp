/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Webserv.cpp                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: jade-haa <jade-haa@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/11 16:45:43 by rfinneru      #+#    #+#                 */
/*   Updated: 2024/11/13 16:26:40 by rfinneru      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Webserv.hpp"

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
		throw std::runtime_error("Unable to open file: " + fileName);
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
			return (i);
	}
	return (-1);
}

Webserv::Webserv(char *fileName)
{
    logger.setWorking(true);
    disable_ctrl_chars();
	setConfig(std::string(fileName));
	_epollFd = epoll_create(1);
	interrupted = 0;
	// _socketsConnectedToServers.reserve(MAX_EVENTS);
}

void Webserv::removeSocketFromReceivedFirstRequest(const int &socket)
{
	auto it = this->_socketReceivedFirstRequest.find(socket);
	if (it != this->_socketReceivedFirstRequest.end())
	{
		this->_socketReceivedFirstRequest.erase(it);
	}
}

int Webserv::handleFirstRequest(const int &client_socket)
{
	int		foundServer;
	char	buffer[BUFFERSIZE];
	int		rd_bytes;

	std::string bufferString;
	rd_bytes = read(client_socket, buffer, BUFFERSIZE);
	buffer[rd_bytes] = '\0';
	if (rd_bytes > 0)
	{
		buffer[rd_bytes] = '\0';
	}
	else if (rd_bytes == 0)
	{
		logger.log(INFO, "Closed socket since 0 bytes read: "
			+ std::to_string(client_socket));
		buffer[rd_bytes] = '\0';
		removeSocketFromReceivedFirstRequest(client_socket);
		removeFdFromEpoll(client_socket);
		close(client_socket);
		return (0);
	}
	else
	{
		logger.log(INFO, "Read failed, closing client socket: "
			+ std::to_string(client_socket));
		buffer[0] = '\0';
		removeSocketFromReceivedFirstRequest(client_socket);
		removeFdFromEpoll(client_socket);
		close(client_socket);
		return (0);
	}
	std::string firstRequest(buffer, rd_bytes);
	foundServer = findRightServer(firstRequest);
	if (foundServer == -1)
	{
		logger.log(ERR, "Couldn't find server for first request");
		removeSocketFromReceivedFirstRequest(client_socket);
		removeFdFromEpoll(client_socket);
		close(client_socket);
		return (0);
	}
	if (_servers[foundServer].initSocketToHandler(client_socket, buffer,
			rd_bytes))
		addSocketToServer(client_socket, &(_servers[foundServer]));
	else
		return (removeSocketFromReceivedFirstRequest(client_socket), 0);
	_servers[foundServer].setFdReadyForWrite(client_socket);
	return (1);
}

Server *Webserv::findServerConnectedToSocket(const int &socket)
{
	CGI_t	*currCGI;

	std::unordered_map<int,
		Server *>::iterator found = _socketsConnectedToServers.find(socket);
	if (found != _socketsConnectedToServers.end())
	{
		return (found->second);
	}
	for (size_t i = 0; i < _servers.size(); i++)
	{
		std::map<int, CGI_t *> &fdsRunningCGI = _servers[i].getFdsRunningCGI();
		for (auto it = fdsRunningCGI.begin(); it != fdsRunningCGI.end(); ++it)
		{
			currCGI = it->second;
			if (currCGI->ReadFd == socket || currCGI->WriteFd == socket)
				return (&(_servers[i]));
		}
	}
	logger.log(ERR, "Couldn't match socket or CGI FD to any server");
	return (nullptr);
}

void Webserv::addFdToReadEpoll(int &socket)
{
	struct epoll_event	eventConfig;

	eventConfig.events = EPOLLIN | EPOLLET;
	eventConfig.data.fd = socket;
	if (epoll_ctl(_epollFd, EPOLL_CTL_ADD, socket, &eventConfig) == -1)
	{
		perror("");
		logger.log(WARNING, "Couldn't add socket to epoll: "
			+ std::to_string(socket));
		close(socket);
	}
}

int Webserv::acceptClientSocket(int &client_socket, socklen_t addrlen,
	const int &server)
{
	client_socket = accept(_servers[server].getSocketFD(),
			(struct sockaddr *)_servers[server].getAddress(), &addrlen);
	if (client_socket == -1)
	{
		logger.log(ERR, "Accept client socket failed, break in main loop");
		return (0);
	}
	return (1);
}

int Webserv::initializeConnection(const socklen_t &addrlen, int &client_socket,
	const int &serverConnectIndex)
{
	if (!acceptClientSocket(client_socket, addrlen, serverConnectIndex))
		return (0);
	if (!makeSocketNonBlocking(client_socket))
	{
		removeFdFromEpoll(client_socket);
		close(client_socket);
		return (0);
	}
	addFdToReadEpoll(client_socket);
	insertSocketIntoReceivedFirstRequest(client_socket);
	return (1);
}

void Webserv::setupServers(socklen_t &addrlen)
{
	logger.log(INFO, "Total amount of servers: " + std::to_string(_servers.size()));
	for (size_t i = 0; i < _servers.size(); i++)
	{
		addrlen = sizeof(_servers[i].getAddress());
		_servers[i].setServer(&_epollFd, &_socketsConnectedToServers);
		logger.log(INFO, "Server " + _servers[i].getServerName()
			+ " started on port " + _servers[i].getPortString());
	}
}

int Webserv::findRightServer(const std::string &buffer)
{
	std::string serverName;
	std::string port;
	std::size_t pos;
	if (buffer.empty())
	{
		logger.log(WARNING, "Buffer is empty");
		return (-1);
	}
	pos = buffer.find("Host: ");
	if (pos == std::string::npos)
	{
		std::cout << "Couldn't find any host" << std::endl;
		return (-1);
	}
	pos += 6;
	std::size_t endPos = buffer.find("\r\n", pos);
	if (endPos == std::string::npos)
	{
		endPos = buffer.length();
	}
	std::string hostLine = buffer.substr(pos, endPos - pos);
	std::size_t portPos = hostLine.find(':');
	if (portPos != std::string::npos)
	{
		serverName = hostLine.substr(0, portPos);
		port = hostLine.substr(portPos + 1);
	}
	else
	{
		serverName = hostLine;
	}
	serverName = trim(serverName);
	port = trim(port);
	for (size_t i = 0; i < _servers.size(); i++)
		if (_servers[i].getServerName() == serverName
			&& _servers[i].getPortString() == port)
			return (i);
	for (size_t i = 0; i < _servers.size(); i++)
		if (_servers[i].getHost() == serverName
			&& _servers[i].getPortString() == port)
			return (i);
	for (size_t i = 0; i < _servers.size(); i++)
		if (_servers[i].getPortString() == port)
			return (i);
	return (-1);
}

void Webserv::removeFdFromEpoll(const int &socket)
{
	logger.log(INFO, "Removing socket from epoll: " + std::to_string(socket));
	if (epoll_ctl(_epollFd, EPOLL_CTL_DEL, socket, NULL) == -1)
	{
		perror("");
		logger.log(WARNING, "Failed to remove socket from epoll: "
			+ std::to_string(socket));
		close(socket);
	}
}

void Webserv::cleanHandlerRequestResponse()
{
	for (size_t i = 0; i < _servers.size(); i++)
	{
		for (size_t j = 0; j < MAX_EVENTS; j++)
			_servers[i].getHTTPHandler(j).cleanHTTPHandler();
	}
}

void Webserv::insertSocketIntoReceivedFirstRequest(const int &socket)
{
	_socketReceivedFirstRequest.insert({socket, false});
}

bool Webserv::getServerReceivedFirstRequest(const int &socket)
{
	bool	saved;

	std::unordered_map<int,
		bool>::iterator it = _socketReceivedFirstRequest.find(socket);
	if (it != _socketReceivedFirstRequest.end())
	{
		saved = it->second;
		it->second = true;
		return (saved);
	}
	logger.log(WARNING, "Couldn't find anything in ServerReceivedFirstRequest, FD is most likely CGI");
	return (true);
}

std::unordered_map<int, Server *> &Webserv::getSocketsConnectedToServers(void)
{
	return (_socketsConnectedToServers);
}

void Webserv::addSocketToServer(const int &socket, Server *server)
{
	logger.log(INFO, "Trying to add socket " + std::to_string(socket)
		+ " to server: " + std::to_string(server->getSocketFD()));
	if (_socketsConnectedToServers.find(socket) != _socketsConnectedToServers.end())
	{
		logger.log(WARNING, "Socket " + std::to_string(socket)
			+ " is already connected to a server.");
		return ;
	}
	auto result = _socketsConnectedToServers.insert({socket, server});
	if (!result.second)
	{
		logger.log(ERR, "Couldn't add socket to servers connected");
		return ;
	}
	logger.log(INFO, "Paired socket: " + std::to_string(socket)
		+ " to Server with FD " + std::to_string(server->getServerFd()));
}

void Webserv::checkCGItimeouts(void)
{
	CGI_t		*currCGI;
	int			currSocket;
	HTTPHandler	*currentHTTPHandler;

	for (size_t i = 0; i < _servers.size(); i++)
	{
		std::map<int, CGI_t *> &fdsRunningCGI = _servers[i].getFdsRunningCGI();
		for (auto it = fdsRunningCGI.begin(); it != fdsRunningCGI.end(); ++it)
		{
			currSocket = it->first;
			currCGI = it->second;
			if (currCGI->StartTime != 0 && currCGI->LastAction != 0
				&& (time(NULL) - currCGI->StartTime > 30 || time(NULL)
					- currCGI->LastAction > 10))
			{
				logger.log(ERR, "[502] CGI script has been timed out");
				currentHTTPHandler = _servers[i].matchSocketToHandler(currSocket);
				currentHTTPHandler->getResponse().status = httpStatusCode::BadGateway;
				_servers[i].makeResponse(PAGE_502, *currentHTTPHandler);
				_servers[i].sendResponse(*currentHTTPHandler, currSocket);
				currentHTTPHandler->setConnectedToCGI(nullptr);
				_servers[i].removeCGIrunning(currSocket);
				if (currCGI->WriteFd >= 0)
				{
					_servers[i].removeFdFromEpoll(currCGI->WriteFd);
					close(currCGI->WriteFd);
				}
				if (currCGI->ReadFd >= 0)
				{
					_servers[i].removeFdFromEpoll(currCGI->ReadFd);
					close(currCGI->ReadFd);
				}
				if (kill(it->second->PID, SIGTERM) == 0)
				{
					usleep(100000);
					kill(it->second->PID, SIGKILL);
				}
				close(currSocket);
				delete currCGI;
				resetCGI(*currCGI);
				break ;
			}
		}
	}
}

void Webserv::initalizeServers(socklen_t &addrlen)
{
	initializeSignals();
	this->setupServers(addrlen);
	for (size_t i = 0; i < _servers.size(); i++)
		_servers.at(i).linkHandlerResponseRequest(&_socketReceivedFirstRequest);
	this->cleanHandlerRequestResponse();
}

void Webserv::cleanUpServers()
{
	for (size_t i = 0; i < _servers.size(); i++)
	{
		while (!_servers[i].getFdsRunningCGI().empty())
		{
			std::map<int,
				CGI_t *>::iterator it = _servers[i].getFdsRunningCGI().begin();
			logger.log(INFO, "Killing CGI process with PID: "
				+ std::to_string(it->second->PID));
			if (kill(it->second->PID, SIGTERM) == 0)
			{
				usleep(200000);
				kill(it->second->PID, SIGKILL);
			}
			close(it->second->ReadFd);
			close(it->second->WriteFd);
			_servers[i].removeCGIrunning(it->first);
			delete it->second;
		}
		close(_servers[i].getSocketFD());
		logger.log(INFO, "Server shut down at port: "
			+ _servers[i].getPortString());
	}
}

Webserv::~Webserv()
{
	if (_epollFd >= 0)
		close(_epollFd);
}