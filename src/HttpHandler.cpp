/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   HttpHandler.cpp                                    :+:    :+:            */
/*                                                     +:+                    */
/*   By: jade-haa <jade-haa@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/13 20:01:28 by jade-haa      #+#    #+#                 */
/*   Updated: 2024/07/31 15:59:02 by rfinneru      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "../include/HttpHandler.hpp"

HttpHandler::HttpHandler() : _request(nullptr), _response(nullptr),
	_foundDirective(nullptr), _server(nullptr), _readCount(0), _totalReadCount(0), _bytesToRead(0), _isCgi(false),
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

Locations *HttpHandler::findMatchingDirective(void)
{
	Locations	*currLongestLocation;

	currLongestLocation = nullptr;
	for (size_t i = 0; i < _server->getLocation().size(); i++)
	{
		const std::string &locationDirective = _server->getLocation()[i].getLocationDirective();
		if (getRequest()->requestDirectory.find(locationDirective) != std::string::npos)
		{
			if (currLongestLocation == nullptr
				|| locationDirective.size() > currLongestLocation->getLocationDirective().size())
				currLongestLocation = &_server->getLocation()[i];
		}
	}
	if (currLongestLocation)
		return (currLongestLocation);
	return (nullptr);
}

void HttpHandler::combineRightUrl(void)
{
	if (!_foundDirective)
	{
		if (checkIfFile(getServer()->getRoot() + getRequest()->requestURL))
		{
			getRequest()->requestURL = getServer()->getRoot()
				+ getRequest()->requestURL;
		}
		else
		{
			logger.log(ERR, "[404] No directory found");
			getResponse()->status = httpStatusCode::NotFound;
			throw NotFoundException();
		}
	}
	else if (_foundDirective->getLocationDirective() == "/")
	{
		if (!_server->getIndex().empty())
			getRequest()->requestURL = _server->getRoot() + _server->getIndex();
		else if (_foundDirective->getAutoIndex())
		{
			getRequest()->requestURL = _server->getRoot();
			_returnAutoIndex = true;
			logger.log(WARNING, "No index found in config file and now trying to use autoindex for /");
		}
		else if(_hasRedirect)
			return;
		else
		{
			logger.log(ERR,
				"[404] No index found in config file and no autoindex in current directive");
			getResponse()->status = httpStatusCode::NotFound;
			throw NotFoundException();
		}
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
				getRequest()->requestURL = _server->getRoot() + getRequest()->requestURL;
				_returnAutoIndex = true;
				logger.log(WARNING, "No index found in foundDirective, now trying autoindex");
			}
			else if (_hasRedirect)
				return;
			else
			{
				logger.log(ERR,
					"[404] No index foundDirective in config file and no autoindex in foundDirective");
				getResponse()->status = httpStatusCode::NotFound;
				throw NotFoundException();
			}
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
	_headerChecked = true;
}

int HttpHandler::pathCheck(void)
{
	std::string dir = getServer()->getRoot() + getRequest()->requestDirectory;
	std::string file = getServer()->getRoot() + getRequest()->requestDirectory
		+ getRequest()->requestFile;
	if (!checkIfDir(dir))
	{
		logger.log(ERR, "[404] Directory doesn't exist");
		getResponse()->status = httpStatusCode::NotFound;
		throw NotFoundException();
	}
	if (getRequest()->requestFile != "")
	{
		if (!checkIfFile(file))
		{
			logger.log(ERR, "[404] File doesn't exist");
			getResponse()->status = httpStatusCode::NotFound;
			throw NotFoundException();
		}
	}
	return (1);
}

void HttpHandler::methodCheck(void)
{
	if (getRequest()->method == ERROR)
	{
		logger.log(ERR, "[400] Only available methods are GET, POST and DELETE");
		getResponse()->status = httpStatusCode::BadRequest;
		throw BadRequestException();
	}
}

void HttpHandler::fileCheck()
{
	if (getRequest()->file.fileName.size() > 256
		|| hasSpecialCharacters(getRequest()->file.fileName))
	{
		logger.log(ERR,
			"[400] Filename has too many characters or has special characters");
		getResponse()->status = httpStatusCode::BadRequest;
		throw BadRequestException();
	}
	_bytesToRead = getRequest()->contentLength;
}

void HttpHandler::setReadCount(int ReadCount)
{
	_readCount = ReadCount;
}

void HttpHandler::addToTotalReadCount(int count)
{
	_totalReadCount += count;
}

int HttpHandler::getReadCount(void)
{
	return(_readCount);
}

int HttpHandler::getTotalReadCount(void)
{
	return (_totalReadCount);	
}

int HttpHandler::getBytesToRead(void)
{
	return (_bytesToRead);
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
	pathCheck();
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
	{
		_hasRedirect = true;
		// std::cout << _foundDirective << _foundDirective->getReturn() << std::endl;
	}
	else if (_foundDirective
		&& _foundDirective->getLocationDirective() == "/cgi-bin")
	{
		_isCgi = true;
		// std::cout << "CGI IS NOW SET TO TRUE" << std::endl;
	}
}

void HttpHandler::checkLocationMethod(void)
{
	// std::cout << "GET" << _foundDirective->getMethods().GET << std::endl;
	// std::cout << "POST" << _foundDirective->getMethods().POST << std::endl;
	// std::cout << "DELETE" << _foundDirective->getMethods().DELETE << std::endl;
	if (getRequest()->method == GET)
	{
		if (!_foundDirective->getMethods().GET)
		{
			logger.log(ERR, "[405] Method not allowed in location");
			getResponse()->status = httpStatusCode::MethodNotAllowed;
			throw MethodNotAllowedException();
		}
	}
	else if (getRequest()->method == POST)
	{
		if (!_foundDirective->getMethods().POST)
		{
			logger.log(ERR, "[405] Method not allowed in location");
			getResponse()->status = httpStatusCode::MethodNotAllowed;
			throw MethodNotAllowedException();
		}
	}
	else if (getRequest()->method == DELETE)
	{
		if (!_foundDirective->getMethods().DELETE)
		{
			logger.log(ERR, "[405] Method not allowed in location");
			getResponse()->status = httpStatusCode::MethodNotAllowed;
			throw MethodNotAllowedException();
		}
	}
}

void HttpHandler::cleanHttpHandler()
{
	_server = nullptr;
	_request = nullptr;
	_response = nullptr;
	_isCgi = false;
	_hasRedirect = false;
	_returnAutoIndex = false;
	_headerChecked = false;
	_readCount = 0;
	_totalReadCount = 0;
}

void HttpHandler::handleRequest(Server &serverAddress, request_t *request,
	response_t *response)
{
	_server = &serverAddress;
	_request = request;
	_response = response;
	_isCgi = false;
	_hasRedirect = false;
	_returnAutoIndex = false;
	_headerChecked = false;
	checkRequestData();
	_foundDirective = findMatchingDirective();
	if (_foundDirective)
		checkLocationMethod();
	setBooleans();
	combineRightUrl();
	// deleteFoundDirective(_foundDirective);
}

Locations *HttpHandler::getFoundDirective(void)
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
