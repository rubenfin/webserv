/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   HttpHandler.cpp                                    :+:    :+:            */
/*                                                     +:+                    */
/*   By: jade-haa <jade-haa@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/13 20:01:28 by jade-haa      #+#    #+#                 */
/*   Updated: 2024/09/16 13:47:15 by rfinneru      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "../include/HttpHandler.hpp"

HttpHandler::HttpHandler() :  _connectedToSocket(-1), _cgiPtr(nullptr), _request(nullptr), _response(nullptr),
	_foundDirective(nullptr), _server(nullptr), _isCgi(false),
	_hasRedirect(false)
{
}

HttpHandler::~HttpHandler()
{
}

void HttpHandler::handleRequestBody(void)
{
	// TODO, store requestBody somewhere on the server, how do we retrieve it?
}
std::shared_ptr<Locations> HttpHandler::findMatchingDirective(void)
{
    std::shared_ptr<Locations> currLongestLocation = nullptr;

    for (size_t i = 0; i < _server->getLocation().size(); i++)
    {
        const std::string &locationDirective = _server->getLocation()[i].getLocationDirective();
        if (getRequest()->requestDirectory.find(locationDirective) != std::string::npos)
        {
            if (!currLongestLocation
                || locationDirective.size() > currLongestLocation->getLocationDirective().size())
            {
                currLongestLocation = std::make_shared<Locations>(_server->getLocation()[i]);
            }
        }
    }
    return currLongestLocation;
}

void HttpHandler::combineRightUrl(void)
{
	if (_foundDirective)
		std::cout << RED << _foundDirective->getAlias() << RESET << std::endl;
	// No location directive found
	if (!_foundDirective)
	{
		// File in root of server, not inside any directory
		if (checkIfFile(getServer()->getRoot() + getRequest()->requestURL))
		{
			getRequest()->requestURL = getServer()->getRoot()
				+ getRequest()->requestURL;
		}
		else
			getServer()->logThrowStatus(_idx, ERR, "[404] No directory found", httpStatusCode::NotFound, NotFoundException());
	}
	else if (_foundDirective->getAlias() != "")
	{
		logger.log(INFO, "Alias has been found, changing paths");
		if (!_foundDirective->getIndex().empty())
			getRequest()->requestURL = _server->getRoot()
				+ _foundDirective->getAlias() + "/"
				+ _foundDirective->getIndex();
		else if (_foundDirective->getAutoIndex())
		{
			getRequest()->requestURL = _server->getRoot() +_foundDirective->getAlias();
			_returnAutoIndex = true;
			logger.log(WARNING, "No index found in foundDirective,  now trying autoindex");
		}
		else if (_hasRedirect)
			return ;
		else
			getServer()->logThrowStatus(_idx, ERR, "[403] No index foundDirective in config file and no autoindex in foundDirective, is your alias ok?", httpStatusCode::Forbidden, ForbiddenException());
	}
	else if (_foundDirective->getLocationDirective() == "/")
	{
		if (!_server->getIndex().empty())
			getRequest()->requestURL = _server->getRoot() + _server->getIndex();
		else if (_foundDirective->getAutoIndex())
		{
			getRequest()->requestURL = _server->getRoot();
			_returnAutoIndex = true;
			logger.log(WARNING,
				"No index found in config file and now trying to use autoindex for /");
				return ;
		}
		else if (_hasRedirect)
			return ;
		else
			getServer()->logThrowStatus(_idx, ERR, "[403] No index found in config file and no autoindex in current directive", httpStatusCode::Forbidden, ForbiddenException());
	}
	else if (_foundDirective->getRoot() != "")
	{
		getRequest()->requestURL = _foundDirective->getRoot()
			+ getRequest()->requestURL;
	}
	else
	{
		if (getRequest()->requestFile != "")
		{
			getRequest()->requestURL = _server->getRoot()
				+ getRequest()->requestDirectory + getRequest()->requestFile;
		}
		else
		{
			if (!_foundDirective->getIndex().empty())
				getRequest()->requestURL = _server->getRoot()
					+ getRequest()->requestURL + "/"
					+ _foundDirective->getIndex();
			else if (_foundDirective->getAutoIndex())
			{
				getRequest()->requestURL = _server->getRoot()
					+ getRequest()->requestURL;
				_returnAutoIndex = true;
				logger.log(WARNING, "No index found in foundDirective, now trying autoindex");
			}
			else if (_hasRedirect)
				return ;
			else
				getServer()->logThrowStatus(_idx, ERR, "[403] No index foundDirective in config file and no autoindex in foundDirective", httpStatusCode::Forbidden, ForbiddenException());
		}
	}
	// logger.log(DEBUG, "requestURL result --> " + getRequest()->requestURL);
}

Server *HttpHandler::getServer(void)
{
	return (_server);
}

request_t *HttpHandler::getRequest(void)
{
	return (_request);
}

response_t *HttpHandler::getResponse(void)
{
	return (_response);
}

bool HttpHandler::getReturnAutoIndex(void)
{
	return (_returnAutoIndex);
}

void HttpHandler::httpVersionCheck(void)
{
	if (getRequest()->http_v != "1.1")
	{
		getResponse()->status = httpStatusCode::httpVersionNotSupported;
		throw HttpVersionNotSupportedException();
	}
}

int HttpHandler::pathCheck(const std::string& dir, const std::string& file)
{
	if (!dir.empty())
	{
		if (!checkIfDir(dir))
			getServer()->logThrowStatus(_idx, ERR, "[404] Directory doesn't exist", httpStatusCode::NotFound, NotFoundException());
	}
	if (!file.empty() && file != dir)
	{
		if (!checkIfFile(file))
			getServer()->logThrowStatus(_idx, ERR, "[404] File doesn't exist", httpStatusCode::NotFound, NotFoundException());

	}
	return (1);
}

void HttpHandler::methodCheck(void)
{
	if (getRequest()->method == ERROR)
		getServer()->logThrowStatus(_idx, ERR, "[400] Only available methods are GET, POST and DELETE", httpStatusCode::BadRequest, BadRequestException());
}

void HttpHandler::fileCheck()
{
	if (getRequest()->file.fileName.size() > 256
		|| hasSpecialCharacters(getRequest()->file.fileName))
		getServer()->logThrowStatus(_idx, ERR, "[400] Filename has too many characters or has special characters", httpStatusCode::BadRequest, BadRequestException());
}

void HttpHandler::setDelete()
{
	// size_t foundEqual = getRequest()->requestBody.find("=");
	// if (foundEqual + 1 == std::string::npos)
	// {
	// 	logger.log(WARNING, "Did not find any well formatted file");
	// 	getResponse()->status = httpStatusCode::BadRequest;
	// 	throw BadRequestException();
	// }
	if (!getRequest()->requestBody.empty())
	{
		getRequest()->file.fileName = getRequest()->requestBody;
		trimFirstChar(getRequest()->file.fileName);
		trimLastChar(getRequest()->file.fileName);
		replaceEncodedSlash(getRequest()->file.fileName);
		logger.log(DEBUG, "File found to delete: "
			+ getRequest()->file.fileName);
	}
}

void HttpHandler::checkRequestData(void)
{
	httpVersionCheck();
	methodCheck();
	if (getRequest()->method == POST && getRequest()->file.fileExists)
		fileCheck();
	if (getRequest()->method == DELETE)
		setDelete();
}
void	deleteFoundDirective(Locations *_foundDirective)
{
	delete	_foundDirective;
}

void HttpHandler::setBooleans(void)
{
	if (_foundDirective && !_foundDirective->getReturn().empty())
		_hasRedirect = true;
	else if (_foundDirective
		&& _foundDirective->getLocationDirective() == "/cgi-bin")
		_isCgi = true;
}

void HttpHandler::checkLocationMethod(void)
{
	if (getRequest()->method == GET)
	{
		if (!getFoundDirective()->getMethods().GET)
			getServer()->logThrowStatus(_idx, ERR, "[405] Method not allowed in location", httpStatusCode::MethodNotAllowed, MethodNotAllowedException());
	}
	else if (getRequest()->method == POST)
	{
		if (!_foundDirective->getMethods().POST)
			getServer()->logThrowStatus(_idx, ERR, "[405] Method not allowed in location", httpStatusCode::MethodNotAllowed, MethodNotAllowedException());
	}
	else if (getRequest()->method == DELETE)
	{
		if (!_foundDirective->getMethods().DELETE)
			getServer()->logThrowStatus(_idx, ERR, "[405] Method not allowed in location", httpStatusCode::MethodNotAllowed, MethodNotAllowedException());
	}
}

void HttpHandler::setFirstRequest(std::string buffer)
{
	_firstRequest = buffer;
}

void HttpHandler::linkToReceivedFirstRequest(std::unordered_map<int, bool> *_socketReceivedFirstRequest)
{
	this->_socketReceivedFirstRequest = _socketReceivedFirstRequest;
}

void HttpHandler::cleanHttpHandler()
{
	resetRequestResponse(*_request, *_response);
	// _connectedToCGI = -1;
	_socketReceivedFirstRequest->erase(_connectedToSocket);
	_connectedToSocket = -1;
	_cgiPtr = nullptr;
	_firstRequest = "";
	_server = nullptr;
	_foundDirective = nullptr;
	_isCgi = false;
	_hasRedirect = false;
	_returnAutoIndex = false;
	_headerChecked = false;
	_isChunked = false;
}

void HttpHandler::connectToRequestResponse(request_t *request,
	response_t *response, int idx)
{
	this->_idx = idx;
	this->_request = request;
	this->_response = response;
}

void HttpHandler::totalPathCheck(void)
{
	if (_foundDirective->getAlias().empty())
	{
		pathCheck(getServer()->getRoot() + getRequest()->requestDirectory, getServer()->getRoot() + getRequest()->requestDirectory
			+ getRequest()->requestFile);
	}
	else 
	{
		std::cout << getRequest()->requestURL  << "|" << _foundDirective->getLocationDirective() << std::endl;
		if (getRequest()->requestURL != _foundDirective->getLocationDirective() && getRequest()->requestURL != _foundDirective->getLocationDirective() + "/")
			getServer()->logThrowStatus(_idx, ERR, "[404] Directory doesn't completely match in alias", httpStatusCode::NotFound, NotFoundException());
		pathCheck(_server->getRoot()
			+ _foundDirective->getAlias(), _server->getRoot()
			+ _foundDirective->getAlias() + "/"
			+ _foundDirective->getIndex());
	}
}

void HttpHandler::setCurrentSocket(int fd)
{
	_currentSocket = fd;
}

void HttpHandler::checkClientBodySize(void)
{
	if (getRequest()->contentLength != 0 && static_cast<long long>(getRequest()->contentLength) > _foundDirective->getClientBodySize())
		getServer()->logThrowStatus(_idx, ERR, "[413] Content-Length exceeded client body size limit", httpStatusCode::PayloadTooLarge, PayloadTooLargeException());
}

void HttpHandler::handleRequest(Server &serverAddress)
{
	_server = &serverAddress;
	_isCgi = false;
	_hasRedirect = false;
	_returnAutoIndex = false;
	checkRequestData();
	_foundDirective = findMatchingDirective();
	if (_foundDirective)
	{
		checkLocationMethod();
		totalPathCheck();
		// checkClientBodySize();
		setBooleans();
	}
	combineRightUrl();
}

std::shared_ptr<Locations> HttpHandler::getFoundDirective(void)
{
	return (_foundDirective);
}

bool HttpHandler::getCgi(void)
{
	return (_isCgi);
}

bool HttpHandler::getRedirect(void)
{
	return (_hasRedirect);
}

bool HttpHandler::getHeaderChecked(void)
{
	return (_headerChecked);
}

bool HttpHandler::getChunked(void)
{
	return (_isChunked);
}

int HttpHandler::getIdx(void)
{
	return(_idx);
}

int &HttpHandler::getConnectedToSocket(void)
{
	return (_connectedToSocket);
}

void HttpHandler::setHeaderChecked(bool boolean)
{
	_headerChecked = boolean;
}

void HttpHandler::setChunked(bool boolean)
{
	_isChunked = boolean;
}

void HttpHandler::setConnectedToSocket(const int& fd)
{
	_connectedToSocket = fd;
}

void HttpHandler::setConnectedToCGI(CGI_t *cgiPtr)
{
	_cgiPtr = cgiPtr;
}

CGI_t * HttpHandler::getConnectedToCGI(void)
{
	return (_cgiPtr);
}