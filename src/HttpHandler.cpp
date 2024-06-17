/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   HttpHandler.cpp                                    :+:    :+:            */
/*                                                     +:+                    */
/*   By: jade-haa <jade-haa@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/13 20:01:28 by jade-haa      #+#    #+#                 */
/*   Updated: 2024/06/17 13:37:34 by rfinneru      ########   odam.nl         */
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
	std::cout << _request << std::endl;
}

void HttpHandler::setMethods(void)
{
	std::cout << _request << std::endl;
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

void HttpHandler::matchResponse(void)
{
	int	i;

	std::vector<Locations> tmp = _server->getLocation();
	i = 0;
	for (const Locations &location : tmp)
	{
		std::cout << _requestURL << " | " << tmp[i].getLocationDirectory() << "|einde" << std::endl;
		if (_requestURL == tmp[i].getLocationDirectory())
		{
			_response = _requestURL;
			std::cout << "dit is response" << _response << std::endl;
			return ;
		}
		i++;
	}
	std::cout << "niks gematcht" << std::endl;
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
	std::cout << _requestContent << std::endl;
	setRequest();
	setMethods();
	_requestURL = findRequestedURL(content);
	std::cout << "URL Found in request" << _requestURL << std::endl;
	matchResponse();
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
	if (_requestURL == "/")
		_requestURL = _server->getIndex();
	return (this->_server->getRoot() +_requestURL);
}

HttpHandler::HttpHandler()
{
}
HttpHandler::~HttpHandler()
{
}