/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Server.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: jade-haa <jade-haa@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/11 17:00:53 by rfinneru      #+#    #+#                 */
/*   Updated: 2024/12/12 12:59:53 by rfinneru      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Server.hpp"

Server::Server(std::string serverContent) : _sessionManager(new SessionManager)
{
	_serverContent = serverContent;
	_http_handler.resize(MAX_EVENTS);
	setServerName();
	setPortHost();
	setRoot();
	setIndex();
	setMethods();
	setErrors();
	setClientBodySize();
	setLocationsRegex(serverContent);
	setUpload();
	logger.log(INFO, "Server port: " + std::to_string(_port));
}

void Server::initializeAddress()
{
	this->_address->sin_family = AF_INET;
	std::string host = getHost();
	struct addrinfo hints, *res;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	if (getaddrinfo(host.c_str(), NULL, &hints, &res) != 0)
	{
		logger.log(ERR, "Failed to resolve hostname: " + host);
		exit(EXIT_FAILURE);
	}
	if (res != nullptr && res->ai_family == AF_INET)
	{
		this->_address->sin_addr = ((struct sockaddr_in *)res->ai_addr)->sin_addr;
	}
	else
	{
		logger.log(ERR, "Resolved address is not IPv4");
		freeaddrinfo(res);
		exit(EXIT_FAILURE);
	}
	freeaddrinfo(res);
	this->_address->sin_port = htons(getPort());
	if (getPort() <= 0 || getPort() > 65535)
	{
		logger.log(ERR, "Not an available port");
		exit(EXIT_FAILURE);
	}
}

void Server::setClientBodySize(void)
{
	std::string bodySizeM = trim(extractValueUntilLocation("client_body_size "));
	if (bodySizeM.empty())
	{
		logger.log(DEBUG, "Default client body size used (5MB)");
		_client_body_size_server = 5242880;
		return ;
	}
	else if (!bodySizeM.empty() && bodySizeM.back() != 'M')
	{
		logger.log(ERR, "Server client body size can only be set in megabytes, please end with M");
		logger.log(DEBUG, "Default client body size used (5MB)");
		_client_body_size_server = 5242880;
		return ;
	}
	bodySizeM.pop_back();
	if (std::stoll(bodySizeM) > 1000000)
	{
		logger.log(ERR, "Server client body size exceeded, max body size is 1000000MB");
		logger.log(DEBUG, "Default client body size used (5MB)");
		_client_body_size_server = 5242880;
		return ;
	}
	_client_body_size_server = std::stoll(bodySizeM) * 1048576;
	logger.log(DEBUG, "Server client body size set to: " + std::to_string(_client_body_size_server));
}

void Server::makeResponseForRedirect(HTTPHandler &handler)
{
	std::string header;
	std::string body;
	logger.log(DEBUG, "in makeResponseForRedirect");
	handler.getResponse().status = httpStatusCode::MovedPermanently;
	std::string message = getHttpStatusMessage(handler.getResponse().status);
	header = "HTTP/1.1 " + message + "\r\n";
	std::string redirectUrl = handler.getFoundDirective()->getReturn();
	if (redirectUrl.substr(0, 4) != "http")
	{
		redirectUrl = "http://" + redirectUrl;
	}
	header += "Location: " + redirectUrl + "\r\n";
	header += "Content-Type: text/html\r\n";
	header += "Content-Length: 0"
				"\r\n";
	header += "\r\n";
	handler.getResponse().response = header + body;
}

void Server::makeResponse(const std::string &buffer, HTTPHandler &handler)
{
	std::string header;
	std::string message = getHttpStatusMessage(handler.getResponse().status);
	header = "HTTP/1.1 " + message + "\r\n";
	if (handler.getRequest().requestFile.find("jpg") != std::string::npos)
		header += "Content-Type: image/jpg\r\n";
	else if (handler.getRequest().requestFile.find("png") != std::string::npos)
		header += "Content-Type: image/png\r\n";
	header += handler.getResponse().responseHeader;
	if (!buffer.empty())
	{
		header += "Content-Length: " + std::to_string(buffer.size())
			+ "\r\n";
	}
	else
		header += "Content-Length: 0\r\n";
	header += "\r\n";
	handler.getResponse().response = header + buffer + "\r\n";
}

void Server::readFile(HTTPHandler& handler)
{
	FileDescriptor *fd = handler.getFDs();
	
	logger.log(DEBUG, "Request URL in readFile(): "
			+ handler.getRequest().requestURL);
	fd->setTotalToRead(getFileSize(handler.getRequest().requestURL, handler));
	handler.getFDs()->setFileFd(open(handler.getRequest().requestURL.data(), std::ios::in | std::ios::binary));
}

void Server::removeSocketAndServer(const int &socket)
{
	std::unordered_map<int,
		Server *>::iterator found = (*_connectedServersPtr).find(socket);
	if (found != (*_connectedServersPtr).end())
	{
		(*_connectedServersPtr).erase(found);
		logger.log(INFO, "Removed socket: " + std::to_string(socket)
			+ " from Server " + std::to_string(getServerFd()));
		return ;
	}
	logger.log(INFO, "Couldn't find socket in connectedSocketsToServers");
}

void Server::readWriteCGI(const int &CGI_FD, HTTPHandler &handler)
{
	char	buffer[BUFFERSIZE];
	int		status;
	int		socket;
	CGI_t	*currCGI;
	int		br;

	status = 0;
	socket = handler.getConnectedToSocket();
	handler.getConnectedToCGI()->LastAction = time(NULL);
	if (handler.getConnectedToCGI()->ReadFd == CGI_FD)
	{
		std::map<int,
			CGI_t *>::iterator it = _fdsRunningCGI.find(handler.getConnectedToSocket());
		if (it == _fdsRunningCGI.end())
		{
			std::cerr << "Error: CGI process not found for socket " << CGI_FD << std::endl;
			return ;
		}
		currCGI = it->second;
		br = read(currCGI->ReadFd, buffer, BUFFERSIZE);
		if (br > 0)
		{
			buffer[br] = '\0';
			logger.log(INFO, "Read " + std::to_string(br) + " bytes from CGI");
			handler.getResponse().response += buffer;
		}
		else if (br == -1)
		{
			logger.log(WARNING, "Read from CGI returned -1");
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
			delete	currCGI;
			if (status != 0)
			{
				logger.log(ERR, "CGI script exited with status: "
					+ std::to_string(check_status(status)));
				handler.getResponse().status = httpStatusCode::InternalServerError;
				makeResponse(getHttpStatusHTML(handler.getResponse().status), handler);
			}
			else
				makeResponse(handler.getResponse().response, handler);
			sendResponse(handler, socket);
			return ;
		}
	}
	else if (handler.getConnectedToCGI()->WriteFd == CGI_FD)
	{
		br = write(CGI_FD, handler.getRequest().requestBody.data(),
				handler.getRequest().requestBody.size());
		handler.getRequest().requestBody = "";
		if (br == -1 || br < BUFFERSIZE)
		{
			removeFdFromEpoll(CGI_FD);
			close(CGI_FD);
			handler.getConnectedToCGI()->WriteFd = -1;
		}
	}
}

void Server::checkIfBin(HTTPHandler &handler)
{
	if (handler.getRequest().contentLength == handler.getRequest().totalBytesRead
		&& handler.getRequest().bin)
	{
		handler.getServer()->logThrowStatus(handler, ERR,
			"[413] Content-Length exceeded client body size limit",
			httpStatusCode::PayloadTooLarge, PayloadTooLargeException());
	}
}

void Server::readFromSocketSuccess(HTTPHandler &handler, const char *buffer,
	const int &bytes_read)
{
	handler.getRequest().currentBytesRead = bytes_read;
	if (!handler.getChunked())
	{
		parse_request(handler.getRequest(), std::string(buffer, bytes_read));
		handler.handleRequest(*this);
		if (bytes_read == BUFFERSIZE)
			handler.setChunked(true);
		handler.getRequest().totalBytesRead += bytes_read
			- (handler.getRequest().requestContent.size()
				- handler.getRequest().requestBody.size());
	}
	else
	{
		logger.log(DEBUG, "Reading chunked request");
		handler.getRequest().file.fileContent = std::string(buffer, bytes_read);
		removeBoundaryLine(handler.getRequest().file.fileContent,
			trim(handler.getRequest().file.fileBoundary));
		handler.getRequest().totalBytesRead += bytes_read;
		checkIfBin(handler);
	}
}

void Server::readFromSocketError(const int &err, HTTPHandler &handler,
	int &socket)
{
	if (err == -1)
	{
		logger.log(ERR, "Read of client socket failed");
		handler.getResponse().status = httpStatusCode::InternalServerError;
		makeResponse(getHttpStatusHTML(handler.getResponse().status), handler);
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
	handler.setConnectedToSocket(-1);
}

int Server::initSocketToHandler(const int &socket, char *buffer, int bytes_rd)
{
	size_t	i;

	i = 0;
	try
	{
		while (i < _http_handler.size())
		{
			HTTPHandler &handler = _http_handler.at(i);
			if (handler.getConnectedToSocket() == -1)
			{
				handler.setConnectedToSocket(socket);
				this->readFromSocketSuccess(handler, buffer, bytes_rd);
				return (1);
			}
			i++;
		}
	}
	catch (const FavIconException &e)
	{
		this->sendFavIconResponse(_http_handler.at(i), socket);
		return (0);
	}
	catch (const HttpException &e)
	{
		this->makeResponse(e.getPageContent(), _http_handler.at(i));
		this->sendResponse(_http_handler.at(i), socket);
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

std::vector<HTTPHandler>& Server::getHTTPHandlers(void)
{
	return (_http_handler);
}

void Server::removeCGIrunning(const int &socket)
{
	std::map<int, CGI_t *>::iterator it;
	it = _fdsRunningCGI.find(socket);
	if (it != _fdsRunningCGI.end())
	{
		logger.log(INFO, "Removed CGI " + std::to_string(socket)
			+ " and socket: " + std::to_string(it->first)
			+ " from fdsRunningCGI");
		_fdsRunningCGI.erase(it);
	}
}

void Server::sendFavIconResponse(HTTPHandler &handler, const int &socket)
{
	std::string buffer("HTTP/1.1 404 Not Found\r\n\r\n"
		+ std::string(PAGE_404));
	logger.log(RESPONSE, buffer);
	if (send(socket, buffer.c_str(), buffer.size(), 0) == -1)
	{
		std::cerr << "send failed with error code " << errno << " (" << strerror(errno) << ")" << std::endl;
		logger.log(ERR, "[500] Failed to send response to client, send()");
	}
	handler.cleanHTTPHandler();
	removeSocketAndServer(socket);
	removeFdFromEpoll(socket);
	close(socket);
}

void Server::sendResponse(HTTPHandler &handler, const int &socket)
{
	logger.log(INFO, "Sending response to client on socket: "
		+ std::to_string(socket));
	logger.log(RESPONSE, handler.getResponse().response);
	if (send(socket, handler.getResponse().response.data(),
			handler.getResponse().response.size(), 0) == -1)
	{
		logger.log(ERR, "[500] Failed to send response to client, socket is most likely closed");
	}
	removeSocketAndServer(socket);
	removeFdFromEpoll(socket);
	close(socket);
	handler.cleanHTTPHandler();
}

Server::~Server()
{
	delete this->_address;
	delete this->_sessionManager;
}
