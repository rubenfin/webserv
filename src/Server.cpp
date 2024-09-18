/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Server.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: jade-haa <jade-haa@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/11 17:00:53 by rfinneru      #+#    #+#                 */
/*   Updated: 2024/09/18 15:48:44 by rfinneru      ########   odam.nl         */
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

    std::string host = getHost();

    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(host.c_str(), NULL, &hints, &res) != 0) {
        logger.log(ERR, "Failed to resolve hostname: " + host);
        exit(EXIT_FAILURE);
    }

    if (res != nullptr && res->ai_family == AF_INET) {
        this->_address->sin_addr = ((struct sockaddr_in*)res->ai_addr)->sin_addr;
    } else {
        logger.log(ERR, "Resolved address is not IPv4");
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

void Server::setServer(int *epollFd, std::unordered_map<int,
	Server *> *connectedServers)
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
	my_epoll_add((*epollFd), _serverFd, EPOLLIN | EPOLLPRI);
	_epollFDptr = epollFd;
	_connectedServersPtr = connectedServers;
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
	if (!_serverName.empty())
	{
		logger.log(INFO, "Server name: " + _serverName);
		return ;
	}
	logger.log(WARNING, "Server name not found in configuration file");
}

void Server::setPortHost(void)
{
	std::string save;
    save = extractValue("listen");
    if (save.empty())
	{
		std::cerr << "No argument for listen" << std::endl;
		throw std::invalid_argument("No listen argument");
	}
    size_t colonPos = save.find(":");
    
    if (colonPos != std::string::npos)
    {
        _portString = save.substr(colonPos + 1);
		_host = save.substr(0, colonPos);
    }
	else 
		throw std::invalid_argument("No : in the listen argument");
    
    std::cout << "PORT STRING: " << _portString << " |" << std::endl;
    std::cout << "HOST STRING: " << _host << " |" << std::endl;
    try {
        _port = std::stoi(_portString);
		if (!(_port >= 0 && _port <= 65535))
		{
			throw std::invalid_argument("Invalid port range: needs to be between 0 and 65535");
		}
    } catch (const std::exception& e) {
        std::cerr << "Invalid port: " << _portString << std::endl;
        throw; 
    }
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

void Server::setError(const std::string &errorPageNumber,
	const std::string &exceptionName)
{
	std::string errorPage = trim(extractValue("error_page " + errorPageNumber
				+ " "));
	if (errorPage.empty())
	{
		return ;
	}
	std::ifstream file(getRoot() + errorPage, std::ios::binary | std::ios::ate);
	if (file.is_open())
	{
		std::streamsize size = file.tellg();
		file.seekg(0, std::ios::beg);
		char *buffer = new char[size + 1];
		if (file.read(buffer, size))
		{
			buffer[size] = '\0';
			HttpException::setCustomPage(exceptionName, buffer);
			logger.log(INFO, "Created an error page for status: "
				+ errorPageNumber);
		}
		else
		{
			std::cerr << "Failed to read file: " << getRoot()
				+ errorPage << std::endl;
		}
		delete[] buffer;
		file.close();
	}
	else
	{
		std::cerr << "Failed to open file: " << getRoot()
			+ errorPage << std::endl;
	}
}

void Server::setErrors(void)
{
	std::unordered_map<std::string, std::string> errorPages = {
		{"200", "OK"},                     // 200 OK
		{"201", "Created"},                // 201 Created
		{"202", "Accepted"},               // 202 Accepted
		{"204", "NoContent"},              // 204 No Content
		{"301", "MovedPermanently"},       // 301 Moved Permanently
		{"302", "Found"},                  // 302 Found
		{"304", "NotModified"},            // 304 Not Modified
		{"400", "BadRequest"},             // 400 Bad Request
		{"401", "Unauthorized"},           // 401 Unauthorized
		{"403", "Forbidden"},              // 403 Forbidden
		{"404", "NotFound"},               // 404 Not Found
		{"405", "MethodNotAllowed"},       // 405 Method Not Allowed
		{"409", "Conflict"},               // 409 Conflict
		{"413", "PayloadTooLarge"},        // 413 Payload Too Large
		{"500", "InternalServerError"},    // 500 Internal Server Error
		{"501", "NotImplemented"},         // 501 Not Implemented
		{"502", "BadGateway"},             // 502 Bad Gateway
		{"503", "ServiceUnavailable"},     // 503 Service Unavailable
		{"505", "HTTPVersionNonSupported"} // 505 HTTP Version Not Supported
	};
	for (const auto &errorPagePair : errorPages)
	{
		const std::string &statusCode = errorPagePair.first;
		const std::string &exceptionName = errorPagePair.second;
		setError(statusCode, exceptionName);
	}
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

long long Server::getClientBodySize(void)
{
	return (_client_body_size_server);
}

std::string Server::getRoot(void)
{
	return (_root);
}

std::string Server::getHost(void)
{
	return (_host);
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

HTTPHandler &Server::getHTTPHandler(const int& index)
{
	return (_http_handler.at(index));
}

struct sockaddr_in *Server::getAddress(void)
{
	return (_address);
}

int Server::getServerFd(void)
{
	return (_serverFd);
}

std::map<int, CGI_t *> &Server::getFdsRunningCGI(void)
{
	return (_fdsRunningCGI);
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

void Server::linkHandlerResponseRequest(std::unordered_map<int, bool> *_socketReceivedFirstRequest)
{
	for (size_t i = 0; i < _http_handler.size(); i++)
	{
		_http_handler.at(i).setIndex(i);
		_http_handler.at(i).linkToReceivedFirstRequest(_socketReceivedFirstRequest);
	}
}

void Server::setClientBodySize(void)
{
	std::string bodySizeM = trim(extractValueUntilLocation("client_body_size "));
	if (bodySizeM.empty())
	{
		logger.log(INFO, "Default client body size used (5MB)");
		_client_body_size_server = 5242880;
		return ;
	}
	else if (!bodySizeM.empty() && bodySizeM.back() != 'M')
	{
		logger.log(ERR, "Server client body size can only be set in megabytes, please end with M");
		logger.log(INFO, "Default client body size used (5MB)");
		_client_body_size_server = 5242880;
		return ;
	}
	bodySizeM.pop_back();
	logger.log(ERR, "SERVER" + bodySizeM);
	if (std::stoll(bodySizeM) > 100)
	{
		logger.log(ERR, "Server client body size exceeded, max body size is 100MB");
		logger.log(INFO, "Default client body size used (5MB)");
		_client_body_size_server = 5242880;
		return ;
	}
	_client_body_size_server = std::stoll(bodySizeM) * 1048576;
	logger.log(ERR, std::to_string(_client_body_size_server));
}

Server::Server(std::string serverContent)
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

void Server::makeResponseForRedirect(HTTPHandler &handler)
{
	std::string header;
	std::string body;
	logger.log(DEBUG, "in makeResponseForRedirect");
	// Use 302 Found for temporary redirects,
	// or 301 Moved Permanently for permanent redirects
	handler.getResponse().status = httpStatusCode::MovedPermanently;
	// or 301 for permanent
	std::string message = getHttpStatusMessage(handler.getResponse().status);
	header = "HTTP/1.1 " + message + "\r\n";
	std::string redirectUrl = handler.getFoundDirective()->getReturn();
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
	handler.getResponse().response = header + body;
}

void Server::makeResponse(const std::string &buffer, HTTPHandler& handler)
{
    std::string header;
    std::string message = getHttpStatusMessage(handler.getResponse().status);
    header = "HTTP/1.1 " + message + "\r\n";
    if (handler.getRequest().requestFile.find("jpg") != std::string::npos)
        header += "Content-Type: image/jpg\r\n";
    else if (handler.getRequest().requestFile.find("png") != std::string::npos)
        header += "Content-Type: image/png\r\n";
    if (!buffer.empty())
    {
        header += "Content-Length: " + std::to_string(buffer.size() + 2) + "\r\n";
    }
    else
        header += "Content-Length: 0\r\n";
    header += "\r\n";
    handler.getResponse().response = header + buffer + "\r\n";
}

long long Server::getFileSize(const std::string &filename, HTTPHandler& handler)
{
	struct stat	sb;

	logger.log(DEBUG, filename + " getting checked by stat");
	if (stat(filename.data(), &sb) == -1)
	{
		perror("stat");
		logger.log(ERR, "[500] stat said |" + filename + "| is not a file");
		handler.getResponse().status = httpStatusCode::InternalServerError;
		throw InternalServerErrorException();
	}
	else
	{
		return ((long long)sb.st_size);
	}
}

void Server::readFile(HTTPHandler &handler)
{
	int			file;
	int			read_bytes;
	char		*buffer;
	long long	fileSize;

	fileSize = getFileSize(handler.getRequest().requestURL, handler);
	buffer = (char *)malloc((fileSize + 1) * sizeof(char));
	logger.log(DEBUG, "Request URL in readFile(): "
		+ handler.getRequest().requestURL);
	file = open(handler.getRequest().requestURL.c_str(), O_RDONLY);
	if (file == -1)
	{
		handler.getResponse().status = httpStatusCode::NotFound;
		throw NotFoundException();
	}
	read_bytes = read(file, buffer, fileSize);
	buffer[read_bytes] = '\0';
	close(file);
	makeResponse(std::string(buffer, read_bytes), handler);
	free(buffer);
}

void Server::sendFavIconResponse(HTTPHandler &handler, int &socket)
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

void Server::addFdToReadEpoll(epoll_event &eventConfig, int &socket)
{
	eventConfig.events = EPOLLIN | EPOLLET;
	eventConfig.data.fd = socket;
	if (epoll_ctl((*_epollFDptr), EPOLL_CTL_ADD, socket, &eventConfig) == -1)
	{
		perror("");
		std::cout << "addFdToReadEpoll fails!" << std::endl;
		close(socket);
	}
}

void Server::removeCGIrunning(int socket)
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




Server::~Server()
{
	delete this->_address;
}
