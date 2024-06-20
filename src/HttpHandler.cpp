/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   HttpHandler.cpp                                    :+:    :+:            */
/*                                                     +:+                    */
/*   By: jade-haa <jade-haa@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/13 20:01:28 by jade-haa      #+#    #+#                 */
/*   Updated: 2024/06/19 12:58:17 by rfinneru      ########   odam.nl         */
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

void HttpHandler::setRequestBody(void)
{
	_requestBody = extractValue("\n\n");
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

void HttpHandler::combineRightUrl(void)
{
	if (!_foundDirective)
	{
		_requestURL = _server->getRoot() + _server->getError404();
		// std::cout << _requestURL << std::endl;
		setHttpStatusCode(httpStatusCode::NotFound); //TODO, BUG WHEN WANTING TO ACCESS 404 PAGE ON PURPOSE
		return ;
	}
	else if (_foundDirective->getRoot() != "")
	{
		std::cout << "eerste " << std::endl;
		_requestURL = _foundDirective->getRoot() + _requestURL;
	}
	else if (_foundDirective->getLocationDirective() == "/")
	{
		_requestURL = _server->getRoot() + _server->getIndex();
		std::cout << "root = /" << _requestURL << std::endl;
	}
	else
	{
		std::cout << "laatste " << std::endl;
			_requestURL = _server->getRoot() + _requestURL + "/"
				+ _foundDirective->getIndex();
	}
	std::cout << "requestURL result --> " << _requestURL << std::endl;
}
std::string HttpHandler::getHttpStatusMessage() const {

switch (_statusCode)
	{
		case httpStatusCode::OK: return "200 OK";
		case httpStatusCode::Created: return "201 Created";
		case httpStatusCode::Accepted: return "202 Accepted";
		case httpStatusCode::NoContent: return "204 No Content";
		case httpStatusCode::MovedPermanently: return "301 Moved Permanently";
		case httpStatusCode::Found: return "302 Found";
		case httpStatusCode::NotModified: return "304 Not Modified";
		case httpStatusCode::BadRequest: return "400 Bad Request";
		case httpStatusCode::Unauthorized: return "401 Unauthorized";
		case httpStatusCode::Forbidden: return "403 Forbidden";
		case httpStatusCode::NotFound: return "404 Not Found";
		case httpStatusCode::InternalServerError: return "500 Internal Server Error";
		case httpStatusCode::NotImplemented: return "501 Not Implemented";
		case httpStatusCode::BadGateway: return "502 Bad Gateway";
		case httpStatusCode::ServiceUnavailable: return "503 Service Unavailable";
		case httpStatusCode::httpVersionNotSupported: return "505 HTTP Version Not Supported";
		default: return "500 Internal Server Error";
	}
}


void HttpHandler::setHttpVersion(void)
{
	_httpVersion = extractValue("HTTP/");
	if (_httpVersion != "1.1")
		setHttpStatusCode(httpStatusCode::httpVersionNotSupported);
	else 
		setHttpStatusCode(httpStatusCode::OK);
}

void HttpHandler::setHttpStatusCode(httpStatusCode code)
{
	_statusCode = code;
}

void HttpHandler::setBoundary(void)
{
	_boundary = extractValue("Content-Type: multipart/form-data; boundary=");
	std::cout << "boundary" << _boundary << std::endl;
}

void HttpHandler::setContentType(void)
{
	_contentType = extractValue("Content-Type: ");
	std::cout << "my content type " << _contentType << std::endl;
}
void HttpHandler::setContentLength(void)
{
	std::string stringContentLength = extractValue("Content-Length: ");
	if (stringContentLength == "")
		return ;
	_contentLength = std::stoul(stringContentLength);
	std::cout << "my content length " << _contentLength << std::endl;
}

void HttpHandler::setDataContent(void)
{
}

void HttpHandler::setData(void)
{
	try
	{
		setHttpVersion();
		setContentType();
		setBoundary();
		setContentLength();
		setDataContent();
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << '\n';
	}
}
void deleteFoundDirective(Locations *_foundDirective)
{
	delete _foundDirective;
}

void HttpHandler::handleRequest(const std::string &content,
	Server &serverAddress)
{
	

	_server = &serverAddress;
	_requestContent = content;
	setRequest();
	setMethods();
	setData();
	_requestURL = findRequestedURL(content);
	_foundDirective = findMatchingDirective();
	combineRightUrl();
	
	std::cout << "requestURL result --> " << _requestURL << std::endl;
	
	deleteFoundDirective(_foundDirective);
}

std::string HttpHandler::setRequestContent(void)
{
	return (_requestContent);
}

std::string HttpHandler::getRequestBody(void)
{
	return (_requestBody);
}

std::string HttpHandler::getHttpVersion(void)
{
	return(_httpVersion);
}

httpStatusCode HttpHandler::getHttpStatusCode(void) const
{
	return (_statusCode);
}

std::string HttpHandler::getResponseContent(void)
{
	return (_responseContent);
}

std::string HttpHandler::getResponseURL(void)
{
	return (_requestURL);
}

std::string HttpHandler::getContentType(void)
{
	return (_contentType);
}
uint64_t HttpHandler::getContentLength(void)
{
	return (_contentLength);
}

HttpHandler::HttpHandler() : _contentLength(0), _statusCode(httpStatusCode::OK)
{
}

HttpHandler::~HttpHandler()
{
}