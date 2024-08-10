#include "../include/Webserv.hpp"

void	handleSigInt(int signal)
{
	if (signal == SIGINT)
	{
		logger.log(ERR, "closed Webserv with SIGINT");
		interrupted = 1;
	}
}

void Webserv::serverActions(const int &idx, int &socket)
{
	if (_servers[0].getHttpHandler(idx)->getReturnAutoIndex())
	{
		_servers[0].makeResponse((char *)_servers[0].returnAutoIndex(_servers[0].getHttpHandler(idx)->getRequest()->requestURL).c_str(),
			idx);
	}
	else if (_servers[0].getHttpHandler(idx)->getRequest()->method == DELETE)
		_servers[0].deleteFileInServer(idx);
	else if (_servers[0].getHttpHandler(idx)->getCgi())
		_servers[0].cgi(_environmentVariables, idx);
	else if (_servers[0].getHttpHandler(idx)->getRedirect())
		_servers[0].makeResponseForRedirect(idx);
	else if (_servers[0].getHttpHandler(idx)->getRequest()->file.fileExists)
		_servers[0].setFileInServer(idx);
	else
		_servers[0].readFile(idx);
	if (_servers[0].getHttpHandler(idx)->getRequest()->currentBytesRead < BUFFERSIZE
		- 1 && !_servers[0].getHttpHandler(idx)->getChunked())
	{
		_servers[0].sendResponse(idx, socket);
	}
}

void Server::clientConnectionFailed(int client_socket, int idx)
{
	logger.log(ERR, "[500] Error in accept()");
	makeResponse((char *)PAGE_500, idx);
	if (send(client_socket,
			getHttpHandler(idx)->getResponse()->response.c_str(),
			getHttpHandler(idx)->getResponse()->response.size(), 0) == -1)
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

int Webserv::acceptClientSocket(int &client_socket, socklen_t addrlen , int serverConnectIndex, const int &i)
{
	client_socket = accept(_servers[serverConnectIndex].getSocketFD(),
			(struct sockaddr *)_servers[serverConnectIndex].getAddress(), &addrlen);
	if (client_socket == -1)
	{
		_servers[serverConnectIndex].clientConnectionFailed(client_socket, i);
		logger.log(ERR, "Accept client socket failed, break in main loop");
		return (0);
	}
	return (1);
}

void Webserv::readFromSocketError(const int &err, const int &idx, int &socket)
{
	if (err == -1)
	{
		logger.log(ERR, "Read of client socket failed");
		_servers[0].getHttpHandler(idx)->getResponse()->status = httpStatusCode::InternalServerError;
		_servers[0].makeResponse(getHttpStatusHTML(_servers[0].getHttpHandler(idx)->getResponse()->status),
			idx);
		removeFdFromEpoll(socket);
		close(socket);
	}
	else if (err == 0)
	{
		logger.log(DEBUG, "Removed socket " + std::to_string(socket)
			+ " from epoll because 0 bytes read");
		removeFdFromEpoll(socket);
		close(socket);
	}
}

void Webserv::readFromSocketSuccess(const int &idx, const char *buffer,
	const int &bytes_read)
{
	
	_servers[0].getHttpHandler(idx)->getRequest()->currentBytesRead = bytes_read;
	std::cout << "request ----->" << _servers[0].getHttpHandler(idx)->getRequest()->requestContent << std::endl;
	// std::cerr << _servers[0].getHttpHandler(idx)->getChunked() << std::endl;
	if (!_servers[0].getHttpHandler(idx)->getChunked())
	{
		parse_request(_servers[0].getHttpHandler(idx)->getRequest(),
			std::string(buffer, bytes_read), idx);
		_servers[0].getHttpHandler(idx)->handleRequest(_servers[0]);
		if (bytes_read == BUFFERSIZE - 1)
			_servers[0].getHttpHandler(idx)->setChunked(true);
	}
	else
		_servers[0].getHttpHandler(idx)->getRequest()->file.fileContent = std::string(buffer,
				bytes_read);
	_servers[0].getHttpHandler(idx)->getRequest()->totalBytesRead += bytes_read;
}

void Webserv::removeFdFromEpoll(int &socket)
{
	if (epoll_ctl(_epollFd, EPOLL_CTL_DEL, socket, NULL) == -1)
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

void Webserv::setFdReadyForRead(epoll_event &eventConfig, int &socket)
{
	eventConfig.events = EPOLLIN | EPOLLET;
	eventConfig.data.fd = socket;
	if (epoll_ctl(_epollFd, EPOLL_CTL_MOD, socket, &eventConfig) == -1)
	{
		perror("");
		std::cout << "Connection with epoll_ctl fails!" << std::endl;
		close(socket);
	}
}

void Webserv::setFdReadyForWrite(epoll_event &eventConfig, int &socket)
{
	eventConfig.events = EPOLLOUT | EPOLLET;
	eventConfig.data.fd = socket;
	if (epoll_ctl(_epollFd, EPOLL_CTL_MOD, socket, &eventConfig) == -1)
	{
		std::cout << "Modify does not work" << std::endl;
		close(socket);
	}
}

int	fd_is_valid(int fd)
{
	return (fcntl(fd, F_GETFD) != -1 || errno != EBADF);
}

void Webserv::readWriteServer(struct epoll_event *eventList, int idx, int client_socket, struct epoll_event eventConfig)
{
	int		client_tmp;
	ssize_t	bytes_read;
	char	buffer[BUFFERSIZE];

	try
	{
		client_tmp = eventList[idx].data.fd;
		if (eventList[idx].events & EPOLLIN)
		{
			bytes_read = read(client_tmp, buffer, BUFFERSIZE - 1);
			if (bytes_read < 1)
			{
				readFromSocketError(bytes_read, idx, client_tmp);
				return;
			}
			buffer[bytes_read] = '\0';
			readFromSocketSuccess(idx, buffer, bytes_read);
			setFdReadyForWrite(eventConfig, client_tmp);
		}
		else if (eventList[idx].events & EPOLLOUT)
		{
			serverActions(idx, client_tmp);
			if (!fd_is_valid(client_tmp))
				return;
			setFdReadyForRead(eventConfig, client_tmp);
		}
	}
	catch (const FavIconException)
	{
		_servers[0].sendFavIconResponse(idx, client_socket);
	}
	catch (const NotFoundException &e)
	{
		_servers[0].sendNotFoundResponse(idx, client_socket);
	}
	catch (const HttpException &e)
	{
		_servers[0].makeResponse(e.getPageContent(), idx);
		_servers[0].sendResponse(idx, client_socket);
	}
}

int Webserv::execute(void)
{
	int					client_socket;
	socklen_t			addrlen;
	request_t			request[MAX_EVENTS];
	response_t			response[MAX_EVENTS];
	int					eventCount;
	struct epoll_event	eventConfig;
	struct epoll_event	eventList[MAX_EVENTS];
	int serverConnectIndex;

	signal(SIGINT, handleSigInt);
	signal(SIGPIPE, SIG_IGN);
	this->setupServers(addrlen);
	this->cleanHandlerRequestResponse();
	while (!interrupted)
	{
		eventCount = epoll_wait(_epollFd, eventList, MAX_EVENTS, 100);
		for (int idx = 0; idx < eventCount; ++idx)
		{
			serverConnectIndex = checkForNewConnection(eventList[idx].data.fd); 
			if (serverConnectIndex >= 0)
			{
				if (!acceptClientSocket(client_socket, addrlen, serverConnectIndex, idx))
					continue ;
				;
				if (!makeSocketNonBlocking(client_socket))
				{
					close(client_socket);
					continue ;
				}
				addFdToReadEpoll(eventConfig, client_socket);
			}
			else
			{
				readWriteServer(eventList, idx, client_socket, eventConfig);
			}
		}
	}
	close(_servers[0].getSocketFD());
	logger.log(INFO, "Server shut down at port: "
		+ _servers[0].getPortString());
	return (0);
}
