#include "../include/Webserv.hpp"

void Server::readWriteServer(epoll_event &event, HTTPHandler &handler)
{
	int		client_tmp;
	ssize_t	bytes_read;
	char	buffer[BUFFERSIZE];

	try
	{
		client_tmp = event.data.fd;
		if (event.events & EPOLLIN)
		{
			bytes_read = read(client_tmp, buffer, BUFFERSIZE);
			if (bytes_read < 1)
			{
				readFromSocketError(bytes_read, handler, client_tmp);
				return ;
			}
			buffer[bytes_read] = '\0';
			readFromSocketSuccess(handler, buffer, bytes_read);
			setFdReadyForWrite(client_tmp);
		}
		else if (event.events & EPOLLOUT)
		{
			if (!serverActions(handler, client_tmp))
				return ;
			setFdReadyForRead(client_tmp);
		}
	}
	catch (const FavIconException &e)
	{
		sendFavIconResponse(handler, client_tmp);
	}
	catch (const HttpException &e)
	{
		makeResponse(e.getPageContent(), handler);
		sendResponse(handler, client_tmp);
	}
	catch (const std::exception &e)
	{
		logger.log(ERR, "[500] Internal Server Error in readWriteServer");
		makeResponse(PAGE_500, handler);
		sendResponse(handler, client_tmp);
	}
}

int Webserv::handleEvent(struct epoll_event *eventList, const int &event_fd, int idx)
{
	HTTPHandler			*currentHTTPHandler;
	Server				*currentServer;

	currentHTTPHandler = nullptr;
	
	if (interrupted)
		return (1) ;

	logger.log(INFO, "Caught an event on socket: "
		+ std::to_string(event_fd));

	if (!getServerReceivedFirstRequest(event_fd))
	{
		if (!handleFirstRequest(event_fd))
			return (0);
		logger.log(INFO, "Handled first request on: "
			+ std::to_string(event_fd));
		return (0) ;
	}

	currentServer = findServerConnectedToSocket(event_fd);
	if (!currentServer)
	{
		logger.log(ERR, "No server found for socket: "
			+ std::to_string(event_fd));
		return (0) ;
	}
	
	currentHTTPHandler = currentServer->matchSocketToHandler(event_fd);
	if (currentHTTPHandler)
	{
		if (currentHTTPHandler->getConnectedToCGI() == nullptr)
			currentServer->readWriteServer(eventList[idx], *currentHTTPHandler);
		else
			currentServer->readWriteCGI(event_fd, *currentHTTPHandler);
	}
	else
		logger.log(ERR, "No HTTP handler found for socket: "
			+ std::to_string(event_fd));
	return (1) ;
}

void Webserv::checkFiles(std::vector<FileDescriptor *>& files)
{
	char buffer[BUFFERSIZE];
	int br = 0;

	for (FileDescriptor* file : files)
	{

		if (file && file->isOpen() && !file->isReadComplete())
		{
			HTTPHandler *handler = file->getHandler();

			br = read(file->getFileFd(), buffer, BUFFERSIZE);
			if (br == -1)
			{
				sendInternalServerError(handler->getConnectedToSocket());
				removeFdFromEpoll(handler->getConnectedToSocket());
				close(handler->getConnectedToSocket());
				continue;
			}
			buffer[br] = '\0';
			handler->getResponse().response.append(buffer, br);
			file->incrementBytesRead(br);
			if (file->isReadComplete())
			{
				handler->getServer()->makeResponse(handler->getResponse().response, *handler);
				handler->getServer()->sendResponse(*handler, handler->getConnectedToSocket());
			}
		}
	}
}

void Webserv::initializeFDs(std::vector<FileDescriptor *>& files)
{
	for (std::size_t i = 0; i < this->_servers.size(); i++)
	{
		std::vector<HTTPHandler>& handlers = _servers[i].getHTTPHandlers();
		for (auto & handler : handlers)
		{
			files.push_back(handler.getFDs());
		}
	}
}

int Webserv::execute(void)
{
	int					client_socket;
	int					eventCount;
	int					serverEvent;
	socklen_t			addrlen;
	struct epoll_event	eventList[MAX_EVENTS];
	std::vector<FileDescriptor *> files(MAX_EVENTS);

	initalizeServers(addrlen);
	initializeFDs(files);

	while (!interrupted) 
	{
		try
		{
			checkCGItimeouts();
			checkFiles(files);
			// logger.log(INFO, "Waiting for events...");
			eventCount = epoll_wait(_epollFd, eventList, MAX_EVENTS, 1);
			for (int idx = 0; idx < eventCount && !interrupted; ++idx)
			{
				serverEvent = checkForNewConnection(eventList[idx].data.fd);
				if (serverEvent >= 0)
				{
					if (!initializeConnection(addrlen, client_socket, serverEvent))
						continue ;
				}
				else
				{
					if (!handleEvent(eventList, eventList[idx].data.fd, idx))
						continue ;
				}
			}
		}
		catch (const std::exception &e)
		{
			logger.log(ERR, "Caught an error inside loop: "
				+ std::string(e.what()));
			sendInternalServerError(client_socket);
			removeSocketFromReceivedFirstRequest(client_socket);
			close(client_socket);
		}
	}
	cleanUpServers();
	logger.log(INFO, "Webserv has been succesfully closed");
	return (0);
}
