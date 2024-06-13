/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpHandler.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jade-haa <jade-haa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/13 20:01:28 by jade-haa          #+#    #+#             */
/*   Updated: 2024/06/13 21:32:37 by jade-haa         ###   ########.fr       */
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

void HttpHandler::getRequest()
{
	std::istringstream iss(_requestContent);
	std::string line;
	if (std::getline(iss, line))
	{
		_request = line;
	}
    
	std::cout << _request << std::endl;
}

void HttpHandler::handleRequest(const std::string &content)
{
	_requestContent = content;
    getRequest();
    
}

std::string HttpHandler::getRequestContent(void)
{
	return (_requestContent);
}
std::string HttpHandler::getResponseContent(void)
{
	return (_responseContent);
}

HttpHandler::HttpHandler()
{
}
HttpHandler::~HttpHandler()
{
}