#include "../include/Webserv.hpp"

void	handleSigInt(int signal)
{
	if (signal == SIGINT)
	{
		logger.log(ERR, "closed Webserv with SIGINT");
		interrupted = 1;
		exit(EXIT_FAILURE);
	}
}

void Webserv::serverActions(int client_socket, request_t request,
	response_t response, int index)
{
	printRequestStruct(&request, index);
	try
	{
		if (request.requestURL == "/favicon.ico")
			throw NotFoundException();
		if (_servers[0].getHttpHandler(index)->getHeaderChecked() == false)
			_servers[0].getHttpHandler(index)->handleRequest(_servers[0],
				&request, &response);
		if (_servers[0].getHttpHandler(index)->getReturnAutoIndex())
			_servers[0].makeResponse((char *)_servers[0].returnAutoIndex(_servers[0].getHttpHandler(index)->getRequest()->requestURL).c_str(),
				index);
		else if (_servers[0].getHttpHandler(index)->getRequest()->method == DELETE)
			_servers[0].deleteFileInServer(index);
		else if (_servers[0].getHttpHandler(index)->getCgi())
			_servers[0].cgi(_environmentVariables, index);
		else if (_servers[0].getHttpHandler(index)->getRedirect())
			_servers[0].makeResponseForRedirect(index);
		else if (_servers[0].getHttpHandler(index)->getRequest()->file.fileExists)
			_servers[0].setFileInServer(index);
		else
			_servers[0].readFile(index);
	}
	catch (const HttpException &e)
	{
		if (_servers[0].getHttpHandler(index)->getResponse()->status == httpStatusCode::NotFound
			&& !_servers[0].getError404().empty())
		{
			_servers[0].getHttpHandler(index)->getRequest()->requestURL = _servers[0].getRoot()
				+ _servers[0].getError404();
			_servers[0].readFile(index);
		}
		else
			_servers[0].makeResponse(e.getPageContent(), index);
	}
	if (_servers[0].getHttpHandler(index)->getTotalReadCount() == _servers[0].getHttpHandler(index)->getBytesToRead()
		|| _servers[0].getHttpHandler(index)->getBytesToRead() == 0
		|| negativeStatusCode(_servers[0].getHttpHandler(index)->getResponse()->status))
	{
		logger.log(RESPONSE, _servers[0].getResponse());
		if (send(client_socket, _servers[0].getResponse().c_str(),
				strlen(_servers[0].getResponse().c_str()), 0) == -1)
			logger.log(ERR, "[500] Failed to send response to client, send()");
		_servers[0].getHttpHandler(index)->cleanHttpHandler();
		resetRequestResponse(request, response);
	}
}
void Server::clientConnectionFailed(int client_socket, int index)
{
	logger.log(ERR, "[500] Error in accept()");
	makeResponse((char *)PAGE_500, index);
	if (send(client_socket, getResponse().c_str(),
			strlen(getResponse().c_str()), 0) == -1)
		logger.log(ERR, "[500] Failed to send response to client, send()");
}

int	make_socket_non_blocking(int sfd)
{
	int	flags;

	flags = fcntl(sfd, F_GETFL, 0);
	if (flags == -1)
	{
		close(sfd);
		perror("fcntl");
		return (0);
	}
	flags |= O_NONBLOCK;
	if (fcntl(sfd, F_SETFL, flags) == -1)
	{
		close(sfd);
		perror("fcntl");
		return (0);
	}
	return (1);
}

int Webserv::acceptClienSocket(int &client_socket, socklen_t addrlen,
	const int &i)
{
	client_socket = accept(_servers[0].getSocketFD(),
			(struct sockaddr *)_servers[0].getAddress(), &addrlen);
	if (client_socket == -1)
	{
		_servers[0].clientConnectionFailed(client_socket, i);
		return (0);
	}
	return (1);
}

void	hier(void)
{
}

int Webserv::execute(void)
{
	int					client_socket;
	char				buffer[BUFFERSIZE];
	ssize_t				read_count;
	socklen_t			addrlen;
	request_t			request[MAX_EVENTS];
	response_t			response[MAX_EVENTS];
	int					eventCount;
	struct epoll_event	eventConfig;
	struct epoll_event	eventList[MAX_EVENTS];
	int					client_tmp;

	signal(SIGINT, handleSigInt);
	addrlen = sizeof(_servers[0].getAddress());
	_servers[0].setServer(_epollFd);
	logger.log(INFO, "Server " + _servers[0].getServerName()
		+ " started on port " + _servers[0].getPortString());
	interrupted = 0;
	for (size_t i = 0; i < MAX_EVENTS; i++)
	{
		_servers[0].getHttpHandler(i)->cleanHttpHandler();
		resetRequestResponse(request[i], response[i]);
	}
	while (!interrupted)
	{
		eventCount = epoll_wait(_epollFd, eventList, MAX_EVENTS, 10);
		for (int i = 0; i < eventCount; ++i)
		{
			if (eventList[i].data.fd == _servers[0].getServerFd())
			{
				// _servers[0].getHttpHandler(i)->cleanHttpHandler();
				// resetRequestResponse(request[i], response[i]);
				for (size_t i = 0; i < MAX_EVENTS; i++)
				{
					_servers[0].getHttpHandler(i)->cleanHttpHandler();
					resetRequestResponse(request[i], response[i]);
				}
				if (!acceptClienSocket(client_socket, addrlen, i))
					break ;
				if (!make_socket_non_blocking(client_tmp))
					continue ;
				eventConfig.events = EPOLLIN | EPOLLET;
				eventConfig.data.fd = client_socket;
				if (epoll_ctl(_epollFd, EPOLL_CTL_ADD, client_socket,
						&eventConfig))
				{
					std::cout << "Connection with epoll_ctl fails!" << std::endl;
					close(client_socket);
				}
				std::cout << "komt hier wel" << std::endl;
			}
			else
			{
				client_tmp = eventList[i].data.fd;
				if (eventList[i].events & EPOLLIN)
				{
					read_count = read(client_tmp, buffer, BUFFERSIZE - 1);
					buffer[read_count] = '\0';
					std::cout << read_count << std::endl;
					std::cout << buffer << std::endl;
					_servers[0].getHttpHandler(i)->setReadCount(read_count);
					if (_servers[0].getHttpHandler(i)->getHeaderChecked() == false)
					{
						parse_request(&request[i], std::string(buffer,
								read_count), i);
						_servers[0].getHttpHandler(i)->addToTotalReadCount(request[i].file.fileContent.size());
					}
					else
					{
						_servers[0].getHttpHandler(i)->addToTotalReadCount(read_count);
						request[i].file.fileContent = std::string(buffer,
								read_count);
					}
					if (read_count == -1)
					{
						logger.log(ERR, "Read of client socket failed");
						_servers[0].getHttpHandler(i)->getResponse()->status = httpStatusCode::InternalServerError;
						_servers[0].makeResponse(getHttpStatusHTML(_servers[0].getHttpHandler(i)->getResponse()->status),
							i);
						close(client_tmp);
					}
					else if (read_count == 0)
					{
						close(client_tmp);
					}
					else
					{
						eventConfig.events = EPOLLOUT | EPOLLET;
						eventConfig.data.fd = client_tmp;
						std::cout << "is nu readen" << std::endl;
						if (epoll_ctl(_epollFd, EPOLL_CTL_MOD, client_tmp,
								&eventConfig))
						{
							std::cout << "Modify does not work" << std::endl;
							close(client_tmp);
						}
					}
				}
				else if (eventList[i].events & EPOLLOUT)
				{
					logger.log(DEBUG,
						"Amount of bytes read from original request: "
						+ std::to_string(read_count));
					hier();
					serverActions(client_socket, request[i], response[i], i);
					eventConfig.events = EPOLLIN | EPOLLET;
					eventConfig.data.fd = client_tmp;
					if (epoll_ctl(_epollFd, EPOLL_CTL_MOD, client_tmp,
							&eventConfig) == -1)
					{
						std::cerr << "epoll_ctl failed with error code " << errno << " (" << strerror(errno) << ")" << std::endl;
						close(client_tmp);
					}
				}
			}
		}
	}
	close(_servers[0].getSocketFD());
	logger.log(INFO, "Server shut down");
	return (0);
}
