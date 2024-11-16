/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   HTTPHandler.cpp                                    :+:    :+:            */
/*                                                     +:+                    */
/*   By: jade-haa <jade-haa@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/13 20:01:28 by jade-haa      #+#    #+#                 */
/*   Updated: 2024/11/14 10:36:31 by rfinneru      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/HTTPHandler.hpp"

HTTPHandler::HTTPHandler() : _connectedToSocket(-1), _cgiPtr(nullptr),
	_idx(-1), _response(), _request(), _foundDirective(nullptr),
	_server(nullptr), _isCgi(false), _hasRedirect(false)
{
}

HTTPHandler::~HTTPHandler()
{
}

void HTTPHandler::setIndex(const int& idx)
{
	_idx = idx;
}

std::shared_ptr<Locations> HTTPHandler::findMatchingDirective(void)
{
	std::shared_ptr<Locations> currLongestLocation = nullptr;
	for (size_t i = 0; i < _server->getLocation().size(); i++)
	{
		const std::string &locationDirective = _server->getLocation()[i].getLocationDirective();
		if (getRequest().requestDirectory.find(locationDirective) != std::string::npos)
		{
			if (!currLongestLocation
				|| locationDirective.size() > currLongestLocation->getLocationDirective().size())
			{
				currLongestLocation = std::make_shared<Locations>(_server->getLocation()[i]);
			}
		}
	}
	return (currLongestLocation);
}

void HTTPHandler::combineRightUrl(void)
{
	if (!_foundDirective)
	{
		// File in root of server, not inside any directory
		if (checkIfFile(getServer()->getRoot() + getRequest().requestURL))
		{
			getRequest().requestURL = getServer()->getRoot()
				+ getRequest().requestURL;
		}
		else
			getServer()->logThrowStatus(*this, ERR, "[404] No directory found",
				httpStatusCode::NotFound, NotFoundException());
	}
	else if (_foundDirective->getAlias() != "")
	{
		logger.log(INFO, "Alias has been found, changing paths");
		if (!_foundDirective->getIndex().empty())
			getRequest().requestURL = _server->getRoot()
				+ _foundDirective->getAlias() + "/"
				+ _foundDirective->getIndex();
		else if (_foundDirective->getAutoIndex())
		{
			getRequest().requestURL = _server->getRoot()
				+ _foundDirective->getAlias();
			_returnAutoIndex = true;
			logger.log(WARNING, "No index found in foundDirective,now trying autoindex");
		}
		else if (_hasRedirect)
			return ;
		else
			getServer()->logThrowStatus(*this, ERR,
				"[403] No index foundDirective in config file and no autoindex in foundDirective", httpStatusCode::Forbidden,
				ForbiddenException());
	} else if (_foundDirective->getRoot() != "")
	{
		if (!_foundDirective->getIndex().empty())
			getRequest().requestURL = _foundDirective->getRoot() + getRequest().requestURL  + "/"
				+ _foundDirective->getIndex();
		else if (_foundDirective->getAutoIndex())
		{
			getRequest().requestURL = _foundDirective->getRoot() + getRequest().requestURL + "/"
				+ _foundDirective->getIndex();
			_returnAutoIndex = true;
			logger.log(WARNING, "No index found in foundDirective, now trying autoindex");
		}
		else if (_hasRedirect)
			return ;
		else
			getServer()->logThrowStatus(*this, ERR,
				"[403] No index foundDirective in config file and no autoindex in foundDirective?", httpStatusCode::Forbidden,
				ForbiddenException());
	}
	else if (_foundDirective->getLocationDirective() == "/")
	{
		if (!_server->getIndex().empty())
			getRequest().requestURL = _server->getRoot() + _server->getIndex();
		else if (_foundDirective->getAutoIndex())
		{
			getRequest().requestURL = _server->getRoot();
			_returnAutoIndex = true;
			logger.log(WARNING,
				"No index found in config file and now trying to use autoindex for /");
			return ;
		}
		else if (_hasRedirect)
			return ;
		else
			getServer()->logThrowStatus(*this, ERR,
				"[403] No index found in config file and no autoindex in current directive",
				httpStatusCode::Forbidden, ForbiddenException());
	}
	else if (_foundDirective->getRoot() != "")
	{
		getRequest().requestURL = _foundDirective->getRoot()
			+ getRequest().requestURL;
	}
	else
	{
		if (getRequest().requestFile != "")
		{
			getRequest().requestURL = _server->getRoot()
				+ getRequest().requestDirectory + getRequest().requestFile;
		}
		else
		{
			if (!_foundDirective->getIndex().empty())
				getRequest().requestURL = _server->getRoot()
					+ getRequest().requestURL + "/"
					+ _foundDirective->getIndex();
			else if (_foundDirective->getAutoIndex())
			{
				getRequest().requestURL = _server->getRoot()
					+ getRequest().requestURL;
				_returnAutoIndex = true;
				logger.log(WARNING, "No index found in foundDirective, now trying autoindex");
			}
			else if (_hasRedirect)
				return ;
			else
				getServer()->logThrowStatus(*this, ERR,
					"[403] No index foundDirective in config file and no autoindex in foundDirective",
					httpStatusCode::Forbidden, ForbiddenException());
		}
	}
	// logger.log(DEBUG, "requestURL result --> " + getRequest().requestURL);
}

Server *HTTPHandler::getServer(void)
{
	return (_server);
}

request_t &HTTPHandler::getRequest(void)
{
	return (_request);
}

response_t &HTTPHandler::getResponse(void)
{
	return (_response);
}

bool HTTPHandler::getReturnAutoIndex(void)
{
	return (_returnAutoIndex);
}

void HTTPHandler::httpVersionCheck(void)
{
	if (getRequest().http_v != "1.1")
	{
		getResponse().status = httpStatusCode::httpVersionNotSupported;
		throw HttpVersionNotSupportedException();
	}
}

int HTTPHandler::pathCheck(const std::string &dir, const std::string &file)
{
	if (!dir.empty())
	{
		if (!checkIfDir(dir))
			getServer()->logThrowStatus(*this, ERR,
				"[404] Directory doesn't exist", httpStatusCode::NotFound,
				NotFoundException());
	}
	if (!file.empty() && file != dir)
	{
		if (!checkIfFile(file))
			getServer()->logThrowStatus(*this, ERR, "[404] File doesn't exist",
				httpStatusCode::NotFound, NotFoundException());
	}
	return (1);
}

void HTTPHandler::methodCheck(void)
{
	if (getRequest().method == ERROR)
		getServer()->logThrowStatus(*this, ERR,
			"[400] Only available methods are GET, POST and DELETE",
			httpStatusCode::BadRequest, BadRequestException());
}

void HTTPHandler::fileCheck()
{
	if (getRequest().file.fileName.size() > 256
		|| hasSpecialCharacters(getRequest().file.fileName))
		getServer()->logThrowStatus(*this, ERR,
			"[400] Filename has too many characters or has special characters",
			httpStatusCode::BadRequest, BadRequestException());
}

void HTTPHandler::setDelete()
{
	// size_t foundEqual = getRequest().requestBody.find("=");
	// if (foundEqual + 1 == std::string::npos)
	// {
	// 	logger.log(WARNING, "Did not find any well formatted file");
	// 	getResponse().status = httpStatusCode::BadRequest;
	// 	throw BadRequestException();
	// }
	if (!getRequest().requestBody.empty())
	{
		getRequest().file.fileName = getRequest().requestBody;
		trimFirstChar(getRequest().file.fileName);
		trimLastChar(getRequest().file.fileName);
		replaceEncodedSlash(getRequest().file.fileName);
		logger.log(DEBUG, "File found to delete: "
			+ getRequest().file.fileName);
	}
}

void HTTPHandler::checkRequestData(void)
{
	httpVersionCheck();
	methodCheck();
	if (getRequest().method == POST && getRequest().file.fileExists)
		fileCheck();
	if (getRequest().method == DELETE)
		setDelete();
}
void	deleteFoundDirective(Locations *_foundDirective)
{
	delete	_foundDirective;
}

void HTTPHandler::setBooleans(void)
{
	if (_foundDirective && !_foundDirective->getReturn().empty())
		_hasRedirect = true;
	else if (_foundDirective
		&& _foundDirective->getLocationDirective() == "/cgi-bin")
		_isCgi = true;
}

void HTTPHandler::checkLocationMethod(void)
{
	if (getRequest().method == GET)
	{
		if (!getFoundDirective()->getMethods().GET)
			getServer()->logThrowStatus(*this, ERR,
				"[405] Method not allowed in location",
				httpStatusCode::MethodNotAllowed, MethodNotAllowedException());
	}
	else if (getRequest().method == POST)
	{
		if (!_foundDirective->getMethods().POST)
			getServer()->logThrowStatus(*this, ERR,
				"[405] Method not allowed in location",
				httpStatusCode::MethodNotAllowed, MethodNotAllowedException());
	}
	else if (getRequest().method == DELETE)
	{
		if (!_foundDirective->getMethods().DELETE)
			getServer()->logThrowStatus(*this, ERR,
				"[405] Method not allowed in location",
				httpStatusCode::MethodNotAllowed, MethodNotAllowedException());
	}
}

void HTTPHandler::setFirstRequest(std::string buffer)
{
	_firstRequest = buffer;
}

void HTTPHandler::linkToReceivedFirstRequest(std::unordered_map<int,
	bool> *_socketReceivedFirstRequest)
{
	this->_socketReceivedFirstRequest = _socketReceivedFirstRequest;
}

void HTTPHandler::cleanHTTPHandler()
{
	resetRequestResponse(_request, _response);
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

void HTTPHandler::totalPathCheck(void)
{
	if (_foundDirective->getAlias().empty())
	{
		if (!_foundDirective->getRoot().empty())
		{
			pathCheck(_foundDirective->getRoot() + getRequest().requestDirectory,
				_foundDirective->getRoot() + getRequest().requestDirectory
				+ getRequest().requestFile);
		}
		else
		{
			pathCheck(getServer()->getRoot() + getRequest().requestDirectory,
				getServer()->getRoot() + getRequest().requestDirectory
				+ getRequest().requestFile);
		}
	}
	else
	{
		if (getRequest().requestURL != _foundDirective->getLocationDirective()
			&& getRequest().requestURL != _foundDirective->getLocationDirective()
			+ "/")
			getServer()->logThrowStatus(*this, ERR,
				"[404] Directory doesn't completely match in alias",
				httpStatusCode::NotFound, NotFoundException());
		pathCheck(_server->getRoot() + _foundDirective->getAlias(),
			_server->getRoot() + _foundDirective->getAlias() + "/"
			+ _foundDirective->getIndex());
	}
}

void HTTPHandler::setCurrentSocket(int fd)
{
	_currentSocket = fd;
}

void HTTPHandler::checkClientBodySize(void)
{
	if (getRequest().contentLength != 0
		&& static_cast<long long>(getRequest().contentLength) > _foundDirective->getClientBodySize())
		{
			getRequest().bin = true;
		}
}

void HTTPHandler::parsingErrorCheck(void)
{
	if (getRequest().internalError)
		getServer()->logThrowStatus(*this, ERR, "[500] Error while parsing request",
			httpStatusCode::InternalServerError, InternalServerErrorException());
}

void HTTPHandler::headerTooLongCheck(void)
{
	if (!getRequest().foundHeaderEnd)
	{
		getServer()->logThrowStatus(*this, ERR, "[431] Header too long",
			httpStatusCode::RequestHeaderTooLong, RequestHeaderTooLongException());
	}
}

void HTTPHandler::favIconCheck(void)
{
	if (getRequest().favIcon)
	{
		getServer()->logThrowStatus(*this, ERR, "[404] FavIcon has not been found", httpStatusCode::NotFound, FavIconException());
	}
}

void HTTPHandler::handleRequest(Server &serverAddress)
{
	favIconCheck();
	parsingErrorCheck();
	headerTooLongCheck();
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
		checkClientBodySize();
		setBooleans();
	}
	combineRightUrl();
}

std::shared_ptr<Locations> HTTPHandler::getFoundDirective(void)
{
	return (_foundDirective);
}

bool HTTPHandler::getCgi(void)
{
	return (_isCgi);
}

bool HTTPHandler::getRedirect(void)
{
	return (_hasRedirect);
}

bool HTTPHandler::getHeaderChecked(void)
{
	return (_headerChecked);
}

bool HTTPHandler::getChunked(void)
{
	return (_isChunked);
}

int HTTPHandler::getIdx(void)
{
	return (_idx);
}

int &HTTPHandler::getConnectedToSocket(void)
{
	return (_connectedToSocket);
}

void HTTPHandler::setHeaderChecked(bool boolean)
{
	_headerChecked = boolean;
}

void HTTPHandler::setChunked(bool boolean)
{
	_isChunked = boolean;
}

void HTTPHandler::setConnectedToSocket(const int &fd)
{
	_connectedToSocket = fd;
}

void HTTPHandler::setConnectedToCGI(CGI_t *cgiPtr)
{
	_cgiPtr = cgiPtr;
}

CGI_t *HTTPHandler::getConnectedToCGI(void)
{
	return (_cgiPtr);
}