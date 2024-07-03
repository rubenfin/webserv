/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   HttpHandler.cpp                                    :+:    :+:            */
/*                                                     +:+                    */
/*   By: jade-haa <jade-haa@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/13 20:01:28 by jade-haa      #+#    #+#                 */
/*   Updated: 2024/07/03 16:38:27 by rfinneru      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "../include/HttpHandler.hpp"

HttpHandler::HttpHandler() : _request(nullptr), _response(nullptr),
	_foundDirective(nullptr), _server(nullptr), _isCgi(false), _hasRedirect(false)
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
			std::cout << locationDirective << " | " << currLongestLocation->getLocationDirective() << std::endl;
		}
	}
	if (currLongestLocation)
		return (currLongestLocation);
	return (nullptr);
}

void HttpHandler::combineRightUrl(void)
{
	if (!_foundDirective || getResponse()->status == httpStatusCode::NotFound)
	{
		// There is no directive found so we return an error.
		getRequest()->requestURL = _server->getRoot() + _server->getError404();
		getResponse()->status = httpStatusCode::NotFound;
		return ;
	}
	else if (_foundDirective->getLocationDirective() == "/")
		getRequest()->requestURL = _server->getRoot() + _server->getIndex();
	else if (_foundDirective->getRoot() != "")
	{
		getRequest()->requestURL = _foundDirective->getRoot()
			+ getRequest()->requestURL;
	}
	else
	{
		if (getRequest()->requestFile != ""
			&& getResponse()->status != httpStatusCode::NotFound)
		{
			getRequest()->requestURL = _server->getRoot()
				+ getRequest()->requestDirectory + getRequest()->requestFile;
		}
		else
		{
			getRequest()->requestURL = _server->getRoot()
				+ getRequest()->requestURL + "/" + _foundDirective->getIndex();
		}
	}
	if (_foundDirective->getLocationDirective() == "/cgi-bin")
	{
		_isCgi = true;
		std::cout << "CGI IS NOW SET TO TRUE" << std::endl;
	}
	std::cout << "requestURL result --> " << getRequest()->requestURL << std::endl;
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

void HttpHandler::httpVersionCheck(void)
{
	if (getRequest()->http_v != "1.1")
		getResponse()->status = httpStatusCode::httpVersionNotSupported;
}

int HttpHandler::pathCheck(void)
{
	std::string dir = getServer()->getRoot() + getRequest()->requestDirectory;
	std::string file = getServer()->getRoot() + getRequest()->requestDirectory
		+ getRequest()->requestFile;
	if (!checkIfDir(dir))
		getResponse()->status = httpStatusCode::NotFound;
	if (getRequest()->requestFile != "")
	{
		if (!checkIfFile(file))
			getResponse()->status = httpStatusCode::NotFound;
		if (access(file.c_str(), X_OK) == -1)
			getResponse()->status = httpStatusCode::NotFound;
	}
	return (1);
}

void HttpHandler::methodCheck(void)
{
	if (getRequest()->method == ERROR)
		getResponse()->status = httpStatusCode::NotFound;
}

void HttpHandler::checkRequestData(void)
{
	try
	{
		httpVersionCheck();
		pathCheck();
		methodCheck();
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << '\n';
	}
}
void	deleteFoundDirective(Locations *_foundDirective)
{
	delete	_foundDirective;
}

void HttpHandler::handleRequest(Server &serverAddress, request_t *request,
	response_t *response)
{
	_server = &serverAddress;
	_request = request;
	_response = response;
	_isCgi = false;
	_hasRedirect = false;
	checkRequestData();
	_foundDirective = findMatchingDirective();
	if (_foundDirective && _foundDirective->getReturn() != "")
		_hasRedirect = true;
	if (_foundDirective)
		std::cout << "returnvalue: " << _foundDirective->getReturn() << std::endl;
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

bool		HttpHandler::getRedirect(void)
{
	return (_hasRedirect);
}
