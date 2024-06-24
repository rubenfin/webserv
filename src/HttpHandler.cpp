/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   HttpHandler.cpp                                    :+:    :+:            */
/*                                                     +:+                    */
/*   By: jade-haa <jade-haa@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/13 20:01:28 by jade-haa      #+#    #+#                 */
/*   Updated: 2024/06/24 19:13:08 by rfinneru      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "../include/HttpHandler.hpp"

HttpHandler::HttpHandler() : _request(nullptr), _response(nullptr), _foundDirective(nullptr),
	_server(nullptr), _isCgi(false)
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
	// TODO: we're checking if the directive is equal,
	// but it should be longest one not equal
	for (size_t i = 0; i < _server->getLocation().size(); i++)
	{
		if (getRequest()->requestURL == _server->getLocation()[i].getLocationDirective())
		{
			std::cout << getRequest()->requestURL << "|" << _server->getLocation()[i].getLocationDirective() << std::endl;
			return (new Locations(_server->getLocation()[i]));
		}
	}
	std::cout << "Found no matching directive" << std::endl;
	return (NULL);
}

void HttpHandler::combineRightUrl(void)
{
	if (!_foundDirective)
	{
		getRequest()->requestURL = _server->getRoot() + _server->getError404();
		getResponse()->status = httpStatusCode::NotFound;
		return ;
	}
	else if (_foundDirective->getRoot() != "")
		getRequest()->requestURL = _foundDirective->getRoot()
			+ getRequest()->requestURL;
	else if (_foundDirective->getLocationDirective() == "/")
		getRequest()->requestURL = _server->getRoot() + _server->getIndex();
	else
	{
		getRequest()->requestURL = _server->getRoot() + getRequest()->requestURL
			+ "/" + _foundDirective->getIndex();
	}
	if (_foundDirective->getLocationDirective() == "/cgi-bin")
	{
		_isCgi = true;
		std::cout << "CGI IS NOW SET TO TRUE" << std::endl;
	}
	std::cout << "requestURL result --> " << getRequest()->requestURL << std::endl;
}

request_t *HttpHandler::getRequest(void)
{
	return (_request);
}

response_t *HttpHandler::getResponse(void)
{
	return (_response);
}

void HttpHandler::checkRequestData(void)
{
	try
	{
		_isCgi = false;
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

void HttpHandler::handleRequest(Server &serverAddress, request_t *request, response_t *response)
{
	_server = &serverAddress;
	_request = request;
	_response = response;
	checkRequestData();
	_foundDirective = findMatchingDirective();
	combineRightUrl();
	deleteFoundDirective(_foundDirective);
}

Locations *HttpHandler::getFoundDirective(void)
{
	return (_foundDirective);
}

bool HttpHandler::getCgi(void)
{
	return (_isCgi);
}
