#include "../include/Webserv.hpp"

int Server::serverActions(const int &idx, int &socket)
{
	if (getHTTPHandler(idx).getReturnAutoIndex())
	{
		makeResponse((char *)returnAutoIndex(idx,
				getHTTPHandler(idx).getRequest().requestURL).c_str(), idx);
	}
	else if (getHTTPHandler(idx).getRequest().method == DELETE)
		deleteFileInServer(idx);
	else if (getHTTPHandler(idx).getCgi())
	{
		cgi(idx, socket);
	}
	else if (getHTTPHandler(idx).getRedirect())
		makeResponseForRedirect(idx);
	else if (getHTTPHandler(idx).getRequest().file.fileExists)
		setFileInServer(idx);
	else
		readFile(idx);
	if (getHTTPHandler(idx).getRequest().currentBytesRead < BUFFERSIZE - 1
		&& !getHTTPHandler(idx).getChunked() && !getHTTPHandler(idx).getCgi())
	{
		sendResponse(idx, socket);
		return (0);
	}
	return (1);
}

void Server::clientConnectionFailed(int client_socket, int idx)
{
	logger.log(ERR, "[500] Error in accept()");
	makeResponse((char *)PAGE_500, idx);
	if (send(client_socket, getHTTPHandler(idx).getResponse().response.c_str(),
			getHTTPHandler(idx).getResponse().response.size(), 0) == -1)
		logger.log(ERR, "[500] Failed to send response to client");
}

int	makeSocketNonBlocking(int &sfd)
{
	int	flags;

	flags = fcntl(sfd, F_SETFL, O_NONBLOCK, 0);
	if (flags == -1)
	{
		perror("fcntl first if");
		return (0);
	}
	return (1);
}

int Webserv::acceptClientSocket(int &client_socket, socklen_t addrlen,
	const int &i, int server)
{
	client_socket = accept(_servers[server].getSocketFD(),
			(struct sockaddr *)_servers[server].getAddress(), &addrlen);
	if (client_socket == -1)
	{
		_servers[server].clientConnectionFailed(client_socket, i);
		logger.log(ERR, "Accept client socket failed, break in main loop");
		return (0);
	}
	return (1);
}

void Server::removeSocketAndServer(int socket)
{
	std::unordered_map<int, Server*>::iterator found = (*_connectedServersPtr).find(socket);

	if (found != (*_connectedServersPtr).end())
	{
		(*_connectedServersPtr).erase(found);
		logger.log(INFO, "Removed socket: " + std::to_string(socket)
			+ " from Server " + std::to_string(getServerFd()));
		return ;
	}
	logger.log(INFO, "Couldn't find socket in connectedSocketsToServers");
}

void Server::readFromSocketError(const int &err, const int &idx, int &socket)
{
	if (err == -1)
	{
		logger.log(ERR, "Read of client socket failed");
		getHTTPHandler(idx).getResponse().status = httpStatusCode::InternalServerError;
		makeResponse(getHttpStatusHTML(getHTTPHandler(idx).getResponse().status),
			idx);
		removeFdFromEpoll(socket);
		close(socket);
	}
	else if (err == 0)
	{
		logger.log(INFO, "Removed socket " + std::to_string(socket)
			+ " from epoll because 0 bytes read");
		removeFdFromEpoll(socket);
		close(socket);
	}
	removeSocketAndServer(socket);
	getHTTPHandler(idx).setConnectedToSocket(-1);
}

void	removeBoundaryLine(std::string &str, const std::string &boundary)
{
	size_t	lineStart;
	size_t	lineEnd;
	size_t	found;

	std::string boundaryLine = boundary + "--";
	found = str.find(boundaryLine);
	if (found != std::string::npos)
	{
		lineStart = str.rfind('\n', found);
		lineStart == std::string::npos ? lineStart = 0 : lineStart += 1;
		lineEnd = str.find('\n', found);
		if (lineEnd == std::string::npos)
		{
			lineEnd = str.length();
		}
		str.erase(lineStart, lineEnd - lineStart + 1);
	}
}

void Server::readFromSocketSuccess(const int &idx, const char *buffer,
	const int &bytes_read)
{
	getHTTPHandler(idx).getRequest().currentBytesRead = bytes_read;
	if (!getHTTPHandler(idx).getChunked())
	{
		parse_request(getHTTPHandler(idx).getRequest(), std::string(buffer,
				bytes_read), idx);
		getHTTPHandler(idx).handleRequest(*this);
		if (bytes_read == BUFFERSIZE)
			getHTTPHandler(idx).setChunked(true);
		getHTTPHandler(idx).getRequest().totalBytesRead += bytes_read
			- (getHTTPHandler(idx).getRequest().requestContent.size()
				- getHTTPHandler(idx).getRequest().requestBody.size());
	}
	else
	{
		logger.log(DEBUG, "Reading chunked request");
		getHTTPHandler(idx).getRequest().file.fileContent = std::string(buffer,
				bytes_read);
		removeBoundaryLine(getHTTPHandler(idx).getRequest().file.fileContent,
			trim(getHTTPHandler(idx).getRequest().file.fileBoundary));
		getHTTPHandler(idx).getRequest().totalBytesRead += bytes_read;
	}
}

void Server::removeFdFromEpoll(int &socket)
{
	logger.log(INFO, "Removing socket from epoll: " + std::to_string(socket));
	if (epoll_ctl((*_epollFDptr), EPOLL_CTL_DEL, socket, NULL) == -1)
	{
		perror("");
		std::cout << "failed to remove fd from epoll: " << socket << std::endl;
		close(socket);
	}
}

void Webserv::addFdToReadEpoll(epoll_event &eventConfig, int &socket)
{
	eventConfig.events = EPOLLIN | EPOLLET;
	eventConfig.data.fd = socket;
	if (epoll_ctl(_epollFd, EPOLL_CTL_ADD, socket, &eventConfig) == -1)
	{
		perror("");
		std::cout << "addFdToReadEpoll fails!" << std::endl;
		close(socket);
	}
}

void Server::setFdReadyForRead(epoll_event &eventConfig, int &socket)
{
	eventConfig.events = EPOLLIN | EPOLLET;
	eventConfig.data.fd = socket;
	if (epoll_ctl((*_epollFDptr), EPOLL_CTL_MOD, socket, &eventConfig) == -1)
	{
		perror("");
		std::cout << "setFdReadyForRead fails!" << std::endl;
		close(socket);
	}
}

void Server::setFdReadyForWrite(epoll_event &eventConfig, const int &socket)
{
	eventConfig.events = EPOLLOUT | EPOLLET;
	eventConfig.data.fd = socket;
	if (epoll_ctl((*_epollFDptr), EPOLL_CTL_MOD, socket, &eventConfig) == -1)
	{
		std::cout << "Modify does not work" << std::endl;
		close(socket);
	}
}

int	fd_is_valid(int fd)
{
	errno = 0;
	return (fcntl(fd, F_GETFD) != -1 || errno != EBADF);
}

void Server::readWriteServer(epoll_event &event, epoll_event &eventConfig,
	HTTPHandler &handler)
{
	int		client_tmp;
	ssize_t	bytes_read;
	char	buffer[BUFFERSIZE];
	int		idx;

	client_tmp = -1;
	idx = handler.getIdx();
	try
	{
		client_tmp = event.data.fd;
		if (event.events & EPOLLIN)
		{
			bytes_read = read(client_tmp, buffer, BUFFERSIZE - 1);
			if (bytes_read < 1)
			{
				readFromSocketError(bytes_read, idx, client_tmp);
				return ;
			}
			buffer[bytes_read] = '\0';
			std::cout << buffer << std::endl;
			readFromSocketSuccess(idx, buffer, bytes_read);
			setFdReadyForWrite(eventConfig, client_tmp);
		}
		else if (event.events & EPOLLOUT)
		{
			if (!serverActions(idx, client_tmp))
				return ;
			setFdReadyForRead(eventConfig, client_tmp);
		}
	}
	catch (const FavIconException &e)
	{
		sendFavIconResponse(idx, client_tmp);
		// setFdReadyForRead(eventConfig, client_tmp);
	}
	catch (const HttpException &e)
	{
		makeResponse(e.getPageContent(), idx);
		sendResponse(idx, client_tmp);
		// setFdReadyForRead(eventConfig, client_tmp);
	}
}

int Server::initSocketToHandler(int &socket, char *buffer, int bytes_rd)
{
	size_t	i;

	i = 0;
	// epoll_event ev;
	try
	{
		while (i < _http_handler.size())
		{
			if (_http_handler.at(i).getConnectedToSocket() == -1)
			{
				_http_handler.at(i).setConnectedToSocket(socket);
				this->readFromSocketSuccess(i, buffer, bytes_rd);
				return (1);
			}
			i++;
		}
	}
	catch (const FavIconException &e)
	{
		this->sendFavIconResponse(i, socket);
		return (0);
	}
	catch (const HttpException &e)
	{
		this->makeResponse(e.getPageContent(), i);
		this->sendResponse(i, socket);
		return (0);
	}
	logger.log(ERR, "Couldn't init socket to handler");
	return (0);
}

HTTPHandler *Server::matchSocketToHandler(const int &socket)
{
	CGI_t	*cgiPtr;

	for (size_t i = 0; i < _http_handler.size(); i++)
	{
		cgiPtr = _http_handler.at(i).getConnectedToCGI();
		// std::cout << "current Socket: " << socket << " found sockets: " << _http_handler.at(i).getConnectedToSocket() << _http_handler.at(i).getConnectedToCGI() << std::endl;
		if (socket == _http_handler.at(i).getConnectedToSocket())
			return (&(_http_handler.at(i)));
		if (cgiPtr != nullptr)
		{
			if (socket == cgiPtr->ReadFd || socket == cgiPtr->WriteFd)
				return (&(_http_handler.at(i)));
		}
	}
	logger.log(ERR, "Couldn't match socket or CGI FD to handler");
	return (nullptr);
}

Server *Webserv::findServerConnectedToSocket(const int &socket)
{
	CGI_t	*currCGI;

	std::unordered_map<int, Server *>::iterator found = _socketsConnectedToServers.find(socket);
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
			// std::cout << currCGI->ReadFd << "|" << socket << std::endl;
			if (currCGI->ReadFd == socket || currCGI->WriteFd == socket)
				return (&(_servers[i]));
		}
	}
	logger.log(ERR, "Couldn't match socket or CGI FD to any server");
	return (nullptr);
}

void	resetCGI(CGI_t &CGI)
{
	CGI.PID = -1;
	CGI.ReadFd = -1;
	CGI.isRunning = false;
	CGI.StartTime = 0;
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
			if (currCGI->StartTime != 0 && time(NULL) - currCGI->StartTime > 10)
			{
				// std::cout << currSocket << "|" << _servers[i].getServerFd() << std::endl;
				logger.log(ERR, "[502] CGI script has been timed out, since it lasted longer than 10 seconds");
				currentHTTPHandler = _servers[i].matchSocketToHandler(currSocket);
				currentHTTPHandler->getResponse().status = httpStatusCode::BadGateway;
				_servers[i].makeResponse(PAGE_502,
					currentHTTPHandler->getIdx());
				_servers[i].sendResponse(currentHTTPHandler->getIdx(),
					currSocket);
				currentHTTPHandler->setConnectedToCGI(nullptr);
				_servers[i].removeCGIrunning(currSocket);
				_servers[i].setFdReadyForRead(_event,
					currentHTTPHandler->getConnectedToSocket());
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
				delete currCGI;
				resetCGI(*currCGI);
				break ;
			}
		}
	}
}

void Server::readWriteCGI(int client_tmp, HTTPHandler &handler)
{
	char	buffer[BUFFERSIZE];
	int		status;
	int		idx;
	int		socket;
	CGI_t	*currCGI;

	status = 0;
	idx = handler.getIdx();
	socket = handler.getConnectedToSocket();
	// std::cout << "client_tmp vs connected CGI to handler " << client_tmp << "|" << handler.getConnectedToCGI() << std::endl;
	if (handler.getConnectedToCGI()->ReadFd == client_tmp)
	{
		std::map<int, CGI_t *>::iterator it = _fdsRunningCGI.find(handler.getConnectedToSocket());
		if (it == _fdsRunningCGI.end())
		{
			std::cerr << "Error: CGI process not found for socket " << client_tmp << std::endl;
			return ;
		}
		currCGI = it->second;
		while (true)
		{
			int br = read(currCGI->ReadFd, buffer, BUFFERSIZE);
			if (br > 0)
			{
				buffer[br] = '\0';
				logger.log(INFO, "Read " + std::to_string(br)
					+ " bytes from CGI");
				handler.getResponse().response += buffer;
			}
			else if (br == 0)
			{
				logger.log(INFO, "EOF reached for CGI");
				waitpid(currCGI->PID, &status, 0);
				removeCGIrunning(handler.getConnectedToSocket());
				handler.setConnectedToCGI(nullptr);
				removeFdFromEpoll(currCGI->ReadFd);
				if (currCGI->WriteFd != -1)
					removeFdFromEpoll(currCGI->WriteFd);
				close(currCGI->ReadFd);
				close(currCGI->WriteFd);
				resetCGI(*currCGI);
				delete currCGI;
				makeResponse(handler.getResponse().response, idx);
				sendResponse(idx, socket);
				return ;
			}
		}
	}
	else if (handler.getConnectedToCGI()->WriteFd == client_tmp)
	{
		int br = write(client_tmp, handler.getRequest().requestBody.data(),
				handler.getRequest().requestBody.size());
		handler.getRequest().requestBody = "";
		if (br == -1 || br < BUFFERSIZE)
		{
			removeFdFromEpoll(client_tmp);
			close(client_tmp);
			handler.getConnectedToCGI()->WriteFd = -1;
		}
	}
}
int Webserv::findRightServer(const std::string &buffer)
{
	std::string serverName;
	std::string port;
	std::size_t pos;
	if (buffer.empty())
	{
		std::cout << "Buffer is empty" << std::endl;
		return (-1);
	}
	// Find the "Host: " header in the buffer
	pos = buffer.find("Host: ");
	if (pos == std::string::npos)
	{
		std::cout << "Couldn't find any host" << std::endl;
		return (-1);
	}
	// Extract the host starting from the position after "Host: " (6 characters ahead)
	pos += 6;                                      // Skip past "Host: "
	std::size_t endPos = buffer.find("\r\n", pos); // Find end of the line
	if (endPos == std::string::npos)
	{
		endPos = buffer.length(); // If no \r\n, take the rest of the buffer
	}
	// Extract the server name and port (if available)
	std::string hostLine = buffer.substr(pos, endPos - pos);
	// Split by ':' to separate server name and port
	std::size_t portPos = hostLine.find(':');
	if (portPos != std::string::npos)
	{
		serverName = hostLine.substr(0, portPos); // Extract server name
		port = hostLine.substr(portPos + 1);      // Extract port
	}
	else
	{
		serverName = hostLine; // No port specified,so entire line is the server name
	}
	// std::cout << "Server Name: " << serverName << std::endl;
	// std::cout << "Port: " << (port.empty() ? "default" : port) << std::endl;
	serverName = trim(serverName);
	port = trim(port);
	// std::cout << _servers.size() << std::endl;
	for (size_t i = 0; i < _servers.size(); i++)
	{
		// std::cout << i << std::endl;
		// std::cout << "|" << _servers[i].getServerName() << "|" << _servers[i].getPortString() << "|" << std::endl;
		if (_servers[i].getServerName() == serverName
			&& _servers[i].getPortString() == port)
		{
			// std::cout << "RETURN: " << i << std::endl;
			return (i);
		}
	}
	return (-1);
		// Temporary return value (success or failure logic can be implemented)
}

void Webserv::removeFdFromEpoll(int &socket)
{
	logger.log(INFO, "Removing socket from epoll: " + std::to_string(socket));
	if (epoll_ctl(_epollFd, EPOLL_CTL_DEL, socket, NULL) == -1)
	{
		perror("");
		std::cout << "failed to remove fd from epoll: " << socket << std::endl;
		close(socket);
	}
}

int Webserv::handleFirstRequest(int &client_socket)
{
	struct epoll_event	eventConfig;
	int					foundServer;
	char				buffer[BUFFERSIZE];
	int					rd_bytes;

	std::string bufferString;
	rd_bytes = read(client_socket, buffer, BUFFERSIZE);
	buffer[rd_bytes] = '\0';
	if (rd_bytes > 0)
	{
		buffer[rd_bytes] = '\0';
	}
	else if (rd_bytes == 0)
	{
		std::cout << "Client closed connection on socket: " << client_socket << std::endl;
		buffer[rd_bytes] = '\0';
		removeFdFromEpoll(client_socket);
		close(client_socket);
	}
	else // rd_bytes == -1
	{
		std::cout << "Read failed, closing client socket: " << client_socket << std::endl;
		buffer[0] = '\0';
		removeFdFromEpoll(client_socket);
		close(client_socket);
	}
	if (!buffer[0])
		return (0);
	std::string firstRequest(buffer, rd_bytes);
	foundServer = findRightServer(firstRequest);
	if (foundServer == -1)
	{
		std::cout << "foundServer is equal to -1";
		removeFdFromEpoll(client_socket);
		close(client_socket);
		return (0);
	}
	if (_servers[foundServer].initSocketToHandler(client_socket, buffer,
			rd_bytes))
		addSocketToServer(client_socket, &(_servers[foundServer]));
	else
	{
		return (0);
	}
	_servers[foundServer].setFdReadyForWrite(eventConfig, client_socket);
	return (1);
}

int Webserv::execute(void)
{
	int					client_socket;
	socklen_t			addrlen;
	int					eventCount;
	struct epoll_event	eventConfig;
	struct epoll_event	eventList[MAX_EVENTS];
	int					serverConnectIndex;
	HTTPHandler			*currentHTTPHandler;
	Server				*currentServer;

	currentHTTPHandler = nullptr;
	initializeSignals();
	this->setupServers(addrlen);
	for (size_t i = 0; i < _servers.size(); i++)
		_servers.at(i).linkHandlerResponseRequest(&_socketReceivedFirstRequest);
	this->cleanHandlerRequestResponse();
	while (!interrupted)
	{
		try
		{
			checkCGItimeouts();
			eventCount = epoll_wait(_epollFd, eventList, MAX_EVENTS, -1);
			for (int idx = 0; idx < eventCount; ++idx)
			{
				serverConnectIndex = checkForNewConnection(eventList[idx].data.fd);
				if (serverConnectIndex >= 0)
				{
					if (!acceptClientSocket(client_socket, addrlen,
							serverConnectIndex, serverConnectIndex))
						continue ;
					if (!makeSocketNonBlocking(client_socket))
					{
						close(client_socket);
						continue ;
					}
					addFdToReadEpoll(eventConfig, client_socket);
					insertSocketIntoReceivedFirstRequest(client_socket);
				}
				else
				{
					if (interrupted)
						break ;
					logger.log(INFO, "Caught an event on socket: "
						+ std::to_string(eventList[idx].data.fd));
					if (!getServerReceivedFirstRequest(eventList[idx].data.fd))
					{
						handleFirstRequest(eventList[idx].data.fd);
						logger.log(INFO, "Handled first request on: "
							+ std::to_string(eventList[idx].data.fd));
						continue ;
					}
					currentServer = findServerConnectedToSocket(eventList[idx].data.fd);
					if (!currentServer)
					{
						logger.log(ERR, "No server found for socket: "
							+ std::to_string(eventList[idx].data.fd));
						continue ;
					}
					currentHTTPHandler = currentServer->matchSocketToHandler(eventList[idx].data.fd);
					if (currentHTTPHandler)
					{
						if (currentHTTPHandler->getConnectedToCGI() == nullptr)
							currentServer->readWriteServer(eventList[idx],
								eventConfig, *currentHTTPHandler);
						else
							currentServer->readWriteCGI(eventList[idx].data.fd,
								*currentHTTPHandler);
					}
					else
						logger.log(ERR, "No HTTP handler found for socket: "
							+ std::to_string(eventList[idx].data.fd));
				}
			}
		}
		catch (const std::exception &e)
		{
			logger.log(ERR, "Caught an error inside loop: "
				+ std::string(e.what()));
			close(client_socket);
		}
	}
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
				sleep(1);
				kill(it->second->PID, SIGKILL);
			}
			close(it->second->ReadFd);
			close(it->second->WriteFd);
			_servers[i].removeCGIrunning(it->first);
		}
		close(_servers[i].getSocketFD());
		logger.log(INFO, "Server shut down at port: "
			+ _servers[i].getPortString());
	}
	return (0);
}
