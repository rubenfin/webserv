#include "../include/Webserv.hpp"

void	handleSigInt(int signal)
{
	if (signal == SIGINT)
	{
		logger.log(ERR, "closed Webserv with SIGINT");
		interrupted = 1;
	}
}

void Server::serverActions(const int &idx, int &socket)
{
	if (getHttpHandler(idx).getReturnAutoIndex())
	{
		makeResponse((char *)returnAutoIndex(idx,
				getHttpHandler(idx).getRequest()->requestURL).c_str(),
			idx);
	}
	else if (getHttpHandler(idx).getRequest()->method == DELETE)
		deleteFileInServer(idx);
	else if (getHttpHandler(idx).getCgi())
		cgi(idx);
	else if (getHttpHandler(idx).getRedirect())
		makeResponseForRedirect(idx);
	else if (getHttpHandler(idx).getRequest()->file.fileExists)
		setFileInServer(idx);
	else
		readFile(idx);
	if (getHttpHandler(idx).getRequest()->currentBytesRead < BUFFERSIZE
		- 1 && !getHttpHandler(idx).getChunked())
	{
		sendResponse(idx, socket);
	}
}

void Server::clientConnectionFailed(int client_socket, int idx)
{
	logger.log(ERR, "[500] Error in accept()");
	makeResponse((char *)PAGE_500, idx);
	if (send(client_socket, getHttpHandler(idx).getResponse()->response.c_str(),
			getHttpHandler(idx).getResponse()->response.size(), 0) == -1)
		logger.log(ERR, "[500] Failed to send response to client");
}

int	makeSocketNonBlocking(int &sfd)
{
	int	flags;

	flags = fcntl(sfd, F_GETFL, 0);
	if (flags == -1)
	{
		perror("fcntl first if");
		return (0);
	}
	flags |= O_NONBLOCK;
	if (fcntl(sfd, F_SETFL, flags) == -1)
	{
		perror("fcntl second if");
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
	auto found = (*_connectedServersPtr).find(socket);

	if (found != (*_connectedServersPtr).end())
	{
		(*_connectedServersPtr).erase(found);
		logger.log(INFO, "Removed socket: " + std::to_string(socket) + " from Server " + std::to_string(getServerFd()));
		return ; 
	}
	logger.log(INFO, "Couldn't find socket in connectedSocketsToServers");
}

void Server::readFromSocketError(const int &err, const int &idx, int &socket)
{
	if (err == -1)
	{
		logger.log(ERR, "Read of client socket failed");
		getHttpHandler(idx).getResponse()->status = httpStatusCode::InternalServerError;
		makeResponse(getHttpStatusHTML(getHttpHandler(idx).getResponse()->status),
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
	getHttpHandler(idx).setConnectedToSocket(-1);
	
}

void	removeBoundaryLine(std::string &str, const std::string &boundary)
{
	size_t	lineStart;
	size_t	lineEnd;
	size_t	found;

	std::string boundaryLine = boundary + "--"; // Expected boundary with "--"
	found = str.find(boundaryLine);
	// Find the first occurrence of the boundary
	if (found != std::string::npos)
	{
		// Find the start of the line
		lineStart = str.rfind('\n', found);
		if (lineStart == std::string::npos)
		{
			lineStart = 0; // If no newline is found, this is the first line
		}
		else
		{
			lineStart += 1; // Move to the character after the newline
		}
		// Find the end of the line
		lineEnd = str.find('\n', found);
		if (lineEnd == std::string::npos)
		{
			lineEnd = str.length(); // If no newline is found,
		}
		// Erase the entire line
		str.erase(lineStart, lineEnd - lineStart + 1); //
		logger.log(INFO, "Removed boundary line: |" + boundary + "|");
	}
	else
	{
		logger.log(INFO, "Did not find boundary line to remove: |" + boundary
			+ "|");
	}
}

void Server::readFromSocketSuccess(const int &idx, const char *buffer,
	const int &bytes_read)
{
	getHttpHandler(idx).getRequest()->currentBytesRead = bytes_read;
	if (!getHttpHandler(idx).getChunked())
	{
		parse_request(getHttpHandler(idx).getRequest(),
			std::string(buffer, bytes_read), idx);
		getHttpHandler(idx).handleRequest(*this);
		if (bytes_read == BUFFERSIZE - 1)
			getHttpHandler(idx).setChunked(true);
		getHttpHandler(idx).getRequest()->totalBytesRead
			+= bytes_read
			- (getHttpHandler(idx).getRequest()->requestContent.size()
				- getHttpHandler(idx).getRequest()->requestBody.size());
	}
	else
	{
		getHttpHandler(idx).getRequest()->file.fileContent = std::string(buffer,
				bytes_read);
		removeBoundaryLine(getHttpHandler(idx).getRequest()->file.fileContent,
			trim(getHttpHandler(idx).getRequest()->file.fileBoundary));
		getHttpHandler(idx).getRequest()->totalBytesRead
			+= bytes_read;
	}
}

void Server::removeFdFromEpoll(int &socket)
{
	if (epoll_ctl((*_epollFDptr), EPOLL_CTL_DEL, socket, NULL) == -1)
	{
		perror("");
		std::cout << "failed to remove fd from epoll" << std::endl;
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
		std::cout << "Connection with epoll_ctl fails!" << std::endl;
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
		std::cout << "Connection with epoll_ctl fails!" << std::endl;
		close(socket);
	}
}

void Server::setFdReadyForWrite(epoll_event &eventConfig, int &socket)
{
	eventConfig.events = EPOLLOUT | EPOLLET;
	eventConfig.data.fd = socket;
	std::cout << _epollFDptr << std::endl;
	std::cout << *_epollFDptr << std::endl;

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

void Server::readWriteServer(epoll_event& event,epoll_event& eventConfig, HttpHandler& handler)
{
	int		client_tmp;
	ssize_t	bytes_read;
	char	buffer[BUFFERSIZE];
	int		idx;

	idx = handler.getIdx();
	try
	{
		// std::cout << "this is a client_tmp: " <<client_tmp << std::endl;
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
			readFromSocketSuccess(idx, buffer, bytes_read);
			setFdReadyForWrite(eventConfig, client_tmp);
		}
		else if (event.events & EPOLLOUT)
		{
			serverActions(idx, client_tmp);
			if (!fd_is_valid(client_tmp))
				return ;
			setFdReadyForRead(eventConfig, client_tmp);
		}
	}
	catch (const FavIconException)
	{
		sendFavIconResponse(idx, client_tmp);
	}
	catch (const HttpException &e)
	{
		makeResponse(e.getPageContent(), idx);
		sendResponse(idx, client_tmp);
	}
}

int Server::initSocketToHandler(const int &socket)
{
	for (size_t i = 0; i < _http_handler.size(); i++)
	{
		if (_http_handler.at(i).getConnectedToSocket() == -1)
		{
			_http_handler.at(i).setConnectedToSocket(socket);
			return (1);
		}
	}
	logger.log(ERR, "Couldn't init socket to handler");
	return (0);
}

HttpHandler *Server::matchSocketToHandler(const int &socket)
{
	for (size_t i = 0; i < _http_handler.size(); i++)
	{
		if (socket == _http_handler.at(i).getConnectedToSocket())
				return (&(_http_handler.at(i)));

	}
	logger.log(ERR, "Couldn't match socket to handler");
	return (nullptr);
}

Server* Webserv::findServerConnectedToSocket(const int& socket)
{
    auto found = _socketsConnectedToServers.find(socket);

    if (found != _socketsConnectedToServers.end())
    {
        return (found->second); 
    }

    logger.log(ERR, "Couldn't match socket to any server");
    return (nullptr);
}


int Webserv::execute(void)
{
	int					client_socket;
	socklen_t			addrlen;
	int					eventCount;
	struct epoll_event	eventConfig;
	struct epoll_event	eventList[MAX_EVENTS];
	int					serverConnectIndex;

	std::vector<request_t> request;
	std::vector<response_t> response;
	signal(SIGINT, handleSigInt);
	signal(SIGPIPE, SIG_IGN);
	this->setupServers(addrlen);
	for (size_t i = 0; i < _servers.size(); i++)
	{
		_servers.at(i).linkHandlerResponseRequest(request, response);
	}
	this->cleanHandlerRequestResponse();
	while (!interrupted)
	{
		try{
		eventCount = epoll_wait(_epollFd, eventList, MAX_EVENTS, -1);
		std::cout << "eventCount: " <<  eventCount << std::endl;
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
				if (_servers[serverConnectIndex].initSocketToHandler(client_socket))
					addSocketToServer(client_socket, &(_servers[serverConnectIndex]));
			}
			else
			{
				std::cout << "evenlist: " <<  eventList[idx].data.fd << std::endl;
			  	Server* currentServer = findServerConnectedToSocket(eventList[idx].data.fd);
				// std::cout << "evennlist[]: " << eventList[idx].data.fd << std::endl;

				HttpHandler *currentHttpHandler = currentServer->matchSocketToHandler(eventList[idx].data.fd);

				if (currentHttpHandler)
					currentServer->readWriteServer(eventList[idx], eventConfig, *currentHttpHandler);
			}
		}}
		catch (const std::exception &e)
		{
			logger.log(ERR, "Catched an error inside loop");
		}
	}
	
	for (size_t i = 0; i < _servers.size(); i++)
	{
		close(_servers[i].getSocketFD());
		logger.log(INFO, "Server shut down at port: "
			+ _servers[i].getPortString());
	}
	return (0);
}
