/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Locations.cpp                                      :+:    :+:            */
/*                                                     +:+                    */
/*   By: jade-haa <jade-haa@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/11 17:08:48 by rfinneru      #+#    #+#                 */
/*   Updated: 2024/09/16 17:04:03 by rfinneru      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Locations.hpp"

void Locations::printMethods(void)
{
	std::cout << "GET Methods == " << _allowedMethods.GET << std::endl;
	std::cout << "POST Methods == " << _allowedMethods.POST << std::endl;
	std::cout << "DELETE Methods == " << _allowedMethods.DELETE << std::endl;
}

void Locations::printLocationsContent(void)
{
	logger.log(DEBUG, _locationContent);
}
void Locations::setMethods(void)
{
	_methodsList = extractValueUntilNL("allow_methods");
	// logger.log(DEBUG, _methodsList);
	_allowedMethods.GET = false;
	_allowedMethods.POST = false;
	_allowedMethods.DELETE = false;
	if (_methodsList.find("GET") != std::string::npos)
		_allowedMethods.GET = true;
	if (_methodsList.find("POST") != std::string::npos)
		_allowedMethods.POST = true;
	if (_methodsList.find("DELETE") != std::string::npos)
		_allowedMethods.DELETE = true;
	// std::cout << "methods " << methodsList << std::endl;
}

void Locations::setAlias(void)
{
	_alias = trim(extractValue("alias "));
}

void Locations::setAutoIndex(void)
{
	_autoindex = false;
	if (trim(extractValue("autoindex ")) == "on")
		_autoindex = true;
	std::cout << (extractValue("autoindex ")) << _autoindex << std::endl;
}

void Locations::setClientBodySize(void)
{
	std::string bodySizeM = trim(extractValue("client_body_size "));
	if (bodySizeM.empty())
	{
		logger.log(INFO, "No location client_body_size set, size set equal to server size");
		_client_body_size = 5242880;
		return;
	}
	else if (!bodySizeM.empty() && bodySizeM.back() != 'M')
	{
		logger.log(ERR, "Client body size can only be set in megabytes, please end with M");
		logger.log(INFO, "Client body size set equal to server size");
		_client_body_size = 5242880;
		return;
	}
	bodySizeM.pop_back();
	logger.log(ERR, bodySizeM);
	if (std::stoll(bodySizeM) > 100)
	{
		logger.log(ERR, "Client body size exceeded, max body size is 100MB");
		logger.log(INFO, "Client body size set equal to server size");
		_client_body_size = 5242880;
		return;
	}
	_client_body_size = std::stoll(bodySizeM) * 1048576;
	logger.log(ERR, std::to_string(_client_body_size));
}

void Locations::getLocationRegex(void)
{
	_locationDirective = extractValue("location");
	setRoot();
	setIndex();
	setCgi_pass();
	setMethods();
	setReturn();
	setAlias();
	setAutoIndex();
	setClientBodySize();
}

std::string Locations::getLocationDirective(void)
{
	return (_locationDirective);
}

MethodsLoc Locations::getMethods(void)
{
	return (_allowedMethods);
}

std::string Locations::getAlias(void)
{
	return (_alias);
}

std::string Locations::getReturn(void)
{
	return (_return);
}

bool Locations::getAutoIndex(void)
{
	return (_autoindex);
}

long long Locations::getClientBodySize(void)
{
	return (_client_body_size);
}

Locations::Locations(std::string locationContent)
{
	this->_url = "test";
	_locationContent = locationContent;
	// std::cout << locationContent << std::endl;
	// std::vector<std::string> content = getLocationStack(locationContent);
	getLocationRegex();
	// for (const std::string &line : content)
	// {
	// 	std::cout << "Location: " << line << std::endl;
	// }
}

Locations::Locations(const Locations *other)
{
	this->_locationContent = other->_locationContent;
	this->_methodsList = other->_methodsList;
	this->_locationDirective = other->_locationDirective;
	this->_allowedMethods = other->_allowedMethods;
	this->_root = other->_root;
	this->_index = other->_index;
	this->_cgi_pass = other->_cgi_pass;
	this->_alias = other->_alias;
	this->_url = other->_url;
	this->_return = other->_return;
}

std::string Locations::extractValueUntilNL(std::string toSearch)
{
	std::size_t keywordPos = _locationContent.find(toSearch);
	if (keywordPos != std::string::npos)
	{
		std::size_t beginLocation = keywordPos + toSearch.size();
		while (beginLocation < _locationContent.size()
			&& std::isspace(_locationContent[beginLocation])
			&& _locationContent[beginLocation] != '\n')
			++beginLocation;
		std::size_t endLocation = beginLocation;
		while (endLocation < _locationContent.size()
			&& _locationContent[endLocation] != '\n')
			++endLocation;
		std::string directory = _locationContent.substr(beginLocation,
				endLocation - beginLocation);
		return (directory);
	}
	return ("");
}

std::string Locations::extractValue(std::string toSearch)
{
	std::size_t keywordPos = _locationContent.find(toSearch);
	if (keywordPos != std::string::npos)
	{
		std::size_t beginLocation = keywordPos + toSearch.size();
		while (beginLocation < _locationContent.size()
			&& std::isspace(_locationContent[beginLocation]))
			++beginLocation;
		std::size_t endLocation = beginLocation;
		while (endLocation < _locationContent.size()
			&& !std::isspace(_locationContent[endLocation]))
			++endLocation;
		std::string directory = _locationContent.substr(beginLocation,
				endLocation - beginLocation);
		return (directory);
	}
	return ("");
}

void Locations::setRoot(void)
{
	_root = extractValue("root");
	// logger.log(DEBUG, _locationDirective);
	if (_locationDirective == "/")
		_root = "files/response_files/";
}
void Locations::setIndex(void)
{
	_index = extractValue("	index");
}
void Locations::setCgi_pass(void)
{
	_cgi_pass = extractValue("cgi_pass");
}

void Locations::setReturn(void)
{
	_return =  extractValue("return");
}

std::string Locations::getRoot(void)
{
	return (_root);
}
std::string Locations::getIndex(void)
{
	return (_index);
}
std::string Locations::getCgi_pass(void)
{
	return (_cgi_pass);
}

Locations::Locations() : _locationContent(""), _methodsList(""),
	_locationDirective(""), _root(""), _index(""), _cgi_pass(""), _alias(""),
	_url(""), _return(""), _autoindex(false)
{
	_allowedMethods.GET = false;
	_allowedMethods.DELETE = false;
	_allowedMethods.POST = false;
}

Locations::~Locations()
{
}