/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpHandler.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jade-haa <jade-haa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/13 20:01:28 by jade-haa          #+#    #+#             */
/*   Updated: 2024/06/17 19:18:27 by jade-haa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/HttpHandler.hpp"

std::string HttpHandler::extractValue(const std::string &toSearch)
{
	std::size_t keywordPos = _requestContent.find(toSearch);
	if (keywordPos != std::string::npos)
	{
		std::size_t beginLocation = keywordPos + toSearch.size();
		while (beginLocation < _requestContent.size()
			&& std::isspace(_requestContent[beginLocation]))
			++beginLocation;
		std::size_t endLocation = beginLocation;
		while (endLocation < _requestContent.size()
			&& !std::isspace(_requestContent[endLocation]))
			++endLocation;
		std::string directory = _requestContent.substr(beginLocation,
				endLocation - beginLocation);
		return (directory);
	}
	return ("");
}

void HttpHandler::setResponseContent(std::string content)
{
	_responseContent = content;
}

void HttpHandler::setRequest()
{
	std::istringstream iss(_requestContent);
	std::string line;
	if (std::getline(iss, line))
	{
		_request = line;
	}
}

void HttpHandler::setMethods(void)
{
	_allowedMethods.GET = false;
	_allowedMethods.POST = false;
	_allowedMethods.DELETE = false;
	if (_request.find("GET") != std::string::npos)
		_allowedMethods.GET = true;
	if (_request.find("POST") != std::string::npos)
		_allowedMethods.POST = true;
	if (_request.find("DELETE") != std::string::npos)
		_allowedMethods.DELETE = true;
	std::cout << "GET " << _allowedMethods.GET << std::endl;
	std::cout << "POST " << _allowedMethods.POST << std::endl;
	std::cout << "DELETE " << _allowedMethods.DELETE << std::endl;
}

Locations *HttpHandler::findMatchingDirective(void)
{
	for (size_t i = 0; i < _server->getLocation().size(); i++)
	{
		if (getResponseURL() == _server->getLocation()[i].getLocationDirective())
			return (new Locations(_server->getLocation()[i]));
	}
	std::cout << "found nothing" << std::endl;
	return (NULL);
}

std::string HttpHandler::findRequestedURL(const std::string &content)
{
	int	i;

	std::string foundRequestedURL;
	i = 0;
	while (content[i])
	{
		if (content[i] == '/')
		{
			foundRequestedURL += content[i];
			i++;
			while (content[i] && content[i] != ' ')
			{
				foundRequestedURL += content[i];
				i++;
			}
			return (foundRequestedURL);
		}
		i++;
	}
	return (foundRequestedURL);
}

void HttpHandler::handleRequest(const std::string &content,
	Server &serverAddress)
{
	_server = &serverAddress;
	_requestContent = content;
	setRequest();
	setMethods();
	_requestURL = findRequestedURL(content);
	_foundDirective = findMatchingDirective();
	if (!_foundDirective)
	{
		_requestURL = _server->getRoot() + _server->getError404();
		std::cerr << "NOT FOUND 404, SEND RESPONSE BACK TODO" << std::endl;
		return ;
	}

	if (_foundDirective->getRoot() != "")
	{
		std::cout << "eerste " << std::endl;
		_requestURL = _foundDirective->getRoot() + _requestURL;
	}
	else if ( _foundDirective->getLocationDirective() == "/")
	{
		_requestURL = _server->getRoot() + _server->getIndex();
		std::cout << "root = /" << _requestURL << std::endl;
	}
	else
	{
		std::cout << "laatste " << std::endl;
		_requestURL = _server->getRoot() + _requestURL + "/" + _foundDirective->getIndex();
	}
	std::cout << "requestURL result --> " << _requestURL << std::endl;
}

std::string HttpHandler::setRequestContent(void)
{
	return (_requestContent);
}
std::string HttpHandler::getResponseContent(void)
{
	return (_responseContent);
}

std::string HttpHandler::getResponseURL(void)
{
	return (_requestURL);
}

HttpHandler::HttpHandler()
{
}
HttpHandler::~HttpHandler()
{
}