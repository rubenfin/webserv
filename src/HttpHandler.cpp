/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpHandler.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jade-haa <jade-haa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/13 20:01:28 by jade-haa          #+#    #+#             */
/*   Updated: 2024/06/14 13:43:52 by jade-haa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/HttpHandler.hpp"

std::string HttpHandler::extractValue(std::string toSearch)
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
	
}

void HttpHandler::handleRequest(const std::string &content)
{
	_requestContent = content;
	std::cout << _requestContent << std::endl;
	setRequest();
	setMethods();
	_requestURL =  extractValue("/");
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

const std::vector<Server>& HttpHandler::getServers() const {
    return _servers;
}

HttpHandler::HttpHandler()
{
}
HttpHandler::~HttpHandler()
{
}