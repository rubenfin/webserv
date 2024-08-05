/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Server.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: jade-haa <jade-haa@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/11 17:00:53 by rfinneru      #+#    #+#                 */
/*   Updated: 2024/08/05 14:55:33 by rfinneru      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"

void	my_epoll_add(int epoll_fd, int fd, uint32_t events)
{
	struct epoll_event	event;

	memset(&event, 0, sizeof(struct epoll_event));
	event.events = events;
	event.data.fd = fd;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event) < 0)
	{
		printf("epoll_ctl\n");
		perror("epoll add");
	}
}

void Server::setSocketOptions(const int &opt)
{
	if (setsockopt(getSocketFD(), SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt,
			sizeof(opt)) < 0)
	{
		perror("setsockopt");
		delete this->_address;
	}
}

void Server::initializeAddress()
{
	this->_address->sin_family = AF_INET;
	this->_address->sin_addr.s_addr = INADDR_ANY;
	// autofills ip address with current host
	this->_address->sin_port = htons(getPort());
	if (getPort() <= 0)
	{
		logger.log(ERR, "Not an available port");
		exit(EXIT_FAILURE);
	}
}

void Server::bindAdressSocket()
{
	if (bind(getSocketFD(), (struct sockaddr *)_address, sizeof(*_address)) < 0)
	{
		logger.log(ERR, "Failed to bind");
		exit(EXIT_FAILURE);
	}
}

void Server::listenToSocket()
{
	if (listen(getSocketFD(), 5) < 0)
	{
		logger.log(ERR, "Failed to listen");
		exit(EXIT_FAILURE);
	}
}

void Server::setServer(int epollFd)
{
	int	opt;

	opt = 1;
	this->_address = new struct sockaddr_in;
	_addrlen = sizeof(*this->_address);
	setSockedFD();
	if (getSocketFD() < 0)
	{
		perror("socket failed");
		delete this->_address;
		exit(EXIT_FAILURE);
	}
	setSocketOptions(opt);
	initializeAddress();
	bindAdressSocket();
	listenToSocket();
	my_epoll_add(epollFd, _serverFd, EPOLLIN | EPOLLPRI);
	for (size_t i = 0; i < MAX_EVENTS; i++)
		_http_handler[i] = new HttpHandler;
	_buffer = (char *)malloc(BUFFERSIZE * sizeof(char));
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
			return (trim(_serverContent.substr(pos, endPos - pos)));
		else
			return (trim(_serverContent.substr(pos)));
	}
	return ("");
}

std::string Server::extractValueUntilLocation(const std::string &searchString)
{
	size_t	pos;
	size_t	endPos;
	size_t	locationPos;

	pos = _serverContent.find(searchString);
	locationPos = _serverContent.find("location");
	if (pos != std::string::npos && (locationPos == std::string::npos
			|| pos < locationPos))
	{
		pos += searchString.length();
		endPos = _serverContent.find('\n', pos);
		if (endPos != std::string::npos && (locationPos == std::string::npos
				|| endPos < locationPos))
			return (trim(_serverContent.substr(pos, endPos - pos)));
		else if (locationPos != std::string::npos)
			return (trim(_serverContent.substr(pos, locationPos - pos)));
		else
			return (trim(_serverContent.substr(pos)));
	}
	return ("");
}
void Server::setServerName(void)
{
	_serverName = extractValue("server_name");
	logger.log(INFO, "Server name: " + _serverName);
}
void Server::setPort(void)
{
	_portString = extractValue("listen");
	_port = std::stoi(_portString);
}
void Server::setRoot(void)
{
	_root = extractValue("root");
	_root.erase(remove_if(_root.begin(), _root.end(), isspace), _root.end());
}
void Server::setIndex(void)
{
	_index = extractValueUntilLocation("index");
	_index.erase(remove_if(_index.begin(), _index.end(), isspace),
		_index.end());
}

void Server::setUpload(void)
{
	_upload = trim(extractValue("upload "));
	if (_upload.empty())
		return ;
	for (size_t i = 0; i < _locations.size(); i++)
	{
		if (_locations[i].getLocationDirective() == _upload)
		{
			logger.log(INFO, "Set upload variable: " + _upload
				+ ",equal to location directive: "
				+ _locations[i].getLocationDirective());
			_upload = getRoot() + _upload;
			return ;
		}
	}
	_upload = "";
	logger.log(WARNING,
		"Found upload variable in configuration file doesn't match any location.");
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
}

void Server::setError404(void)
{
	_error404 = extractValue("error_page 404 ");
}

void Server::printMethods(void)
{
	std::cout << "GET Methods == " << _allowedMethods.GET << std::endl;
	std::cout << "POST Methods == " << _allowedMethods.POST << std::endl;
	std::cout << "DELETE Methods == " << _allowedMethods.DELETE << std::endl;
}

void Server::setSockedFD()
{
	this->_serverFd = socket(AF_INET, SOCK_STREAM, 0);
}

std::string Server::getError404(void)
{
	return (_error404);
}

int Server::getSocketFD(void)
{
	return (this->_serverFd);
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

int Server::getPort(void)
{
	return (_port);
}

std::string Server::getRoot(void)
{
	return (_root);
}

std::string Server::getUpload(void)
{
	return (_upload);
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

HttpHandler *Server::getHttpHandler(int index)
{
	return (_http_handler[index]);
}

struct sockaddr_in *Server::getAddress(void)
{
	return (_address);
}

int Server::getServerFd(void)
{
	return (_serverFd);
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
	std::string locationsStrings[1000]; // NEED TO FIX MAYBE?
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

void Server::linkHandlerResponseRequest(request_t *request,
	response_t *response)
{
	for (size_t i = 0; i < MAX_EVENTS; i++)
		getHttpHandler(i)->connectToRequestResponse(&request[i], &response[i],
			i);
}

Server::Server(std::string serverContent)
{
	_serverContent = serverContent;
	setServerName();
	setPort();
	setRoot();
	setIndex();
	setMethods();
	setError404();
	setLocationsRegex(serverContent);
	setUpload();
	logger.log(INFO, "Server port: " + std::to_string(_port));
}

void Server::makeResponseForRedirect(int idx)
{
	std::string header;
	std::string body;
	logger.log(DEBUG, "in makeResponseForRedirect");
	// Use 302 Found for temporary redirects,
	// or 301 Moved Permanently for permanent redirects
	getHttpHandler(idx)->getResponse()->status = httpStatusCode::MovedPermanently;
	// or 301 for permanent
	std::string message = getHttpStatusMessage(getHttpHandler(idx)->getResponse()->status);
	header = "HTTP/1.1 " + message + "\r\n";
	std::string redirectUrl = getHttpHandler(idx)->getFoundDirective()->getReturn();
	if (redirectUrl.substr(0, 4) != "http")
	{
		redirectUrl = "http://" + redirectUrl;
		// Ensure the URL includes the protocol
	}
	header += "Location: " + redirectUrl + "\r\n";
	header += "Content-Type: text/html\r\n";
	header += "Content-Length: 0"
				"\r\n";
	header += "\r\n";
	getHttpHandler(idx)->getResponse()->response = header + body;
}

void Server::makeResponse(char *buffer, int idx)
{
	std::string header;
	std::string body;
	std::string message = getHttpStatusMessage(getHttpHandler(idx)->getResponse()->status);
	header = "HTTP/1.1 " + message + "\r\n";
	if (buffer)
	{
		body = buffer;
		header += "Content-Length: " + std::to_string(body.length()) + "\r\n";
	}
	else
		header += "Content-Length: 0";
	header += "\r\n";
	getHttpHandler(idx)->getResponse()->response = header + body;
}

void Server::readFile(int idx)
{
	int	file;
	int	rdbytes;

	logger.log(DEBUG, "Request URL in readFile() "
		+ getHttpHandler(idx)->getRequest()->requestURL);
	file = open(getHttpHandler(idx)->getRequest()->requestURL.c_str(),
			O_RDONLY);
	if (file == -1)
	{
		perror("opening file of responseURL");
		return ;
	}
	rdbytes = read(file, _buffer, BUFFERSIZE - 1);
	_buffer[rdbytes] = '\0';
	close(file);
	makeResponse(_buffer, idx);
}

void Server::sendFavIconResponse(const int &idx, int &socket)
{
	std::string buffer("HTTP/1.1 404 Not Found\r\n\r\n"
		+ std::string(PAGE_404));
	logger.log(RESPONSE, buffer);
	if (send(socket, buffer.c_str(), buffer.size(), 0) == -1)
	{
		std::cerr << "send failed with error code " << errno << " (" << strerror(errno) << ")" << std::endl;
		logger.log(ERR, "[500] Failed to send response to client, send()");
	}
	getHttpHandler(idx)->cleanHttpHandler();
}

void Server::sendNotFoundResponse(const int &idx, int &socket)
{
	if (this->getHttpHandler(idx)->getResponse()->status == httpStatusCode::NotFound
		&& !this->getError404().empty())
	{
		this->getHttpHandler(idx)->getRequest()->requestURL = this->getRoot()
			+ this->getError404();
		this->readFile(idx);
	}
	else
		this->makeResponse(PAGE_404, idx);
	this->sendResponse(idx, socket);
	getHttpHandler(idx)->cleanHttpHandler();
}

Server::~Server()
{
	for (size_t i = 0; i < MAX_EVENTS; i++)
		delete _http_handler[i];
	free(_buffer);
	delete this->_address;
}
