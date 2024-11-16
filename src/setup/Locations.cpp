/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Locations.cpp                                      :+:    :+:            */
/*                                                     +:+                    */
/*   By: jade-haa <jade-haa@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/11 17:08:48 by rfinneru      #+#    #+#                 */
/*   Updated: 2024/09/20 13:31:22 by rfinneru      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Locations.hpp"

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
	if (_methodsList.find("GET") != std::string::npos)
		_allowedMethods.GET = true;
	if (_methodsList.find("POST") != std::string::npos)
		_allowedMethods.POST = true;
	if (_methodsList.find("DELETE") != std::string::npos)
		_allowedMethods.DELETE = true;
}

void Locations::setAlias(void)
{
	_alias = trim(extractValue("alias "));
	if (!_alias.empty())
	{
		logger.log(DEBUG, "Alias has been set to: " + _alias + " in location: " + this->_locationDirective);
	}
}

void Locations::setAutoIndex(void)
{
	_autoindex = false;
	if (trim(extractValue("autoindex ")) == "on")
	{
		_autoindex = true;
		logger.log(DEBUG, "Autoindex turned on for location: " + this->_locationDirective);
	}
}

void Locations::setClientBodySize(void)
{
	std::string bodySizeM = trim(extractValue("client_body_size "));
	if (bodySizeM.empty())
	{
		logger.log(DEBUG, "No location client_body_size set, size set equal to server size");
		_client_body_size = 5242880;
		return;
	}
	else if (!bodySizeM.empty() && bodySizeM.back() != 'M')
	{
		logger.log(ERR, "Client body size can only be set in megabytes, please end with M");
		logger.log(DEBUG, "Client body size set equal to server size");
		_client_body_size = 5242880;
		return;
	}
	bodySizeM.pop_back();
	if (std::stoll(bodySizeM) > 1000000)
	{
		logger.log(ERR, "Client body size exceeded, max body size is 1000000MB");
		logger.log(DEBUG, "Client body size set equal to server size");
		_client_body_size = 5242880;
		return;
	}
	_client_body_size = std::stoll(bodySizeM) * 1048576;
}

void Locations::checkIfBothAliasAndRoot()
{
	if (!_root.empty() && !_alias.empty())
	{
		logger.log(ERR, "Using both root and alias in the same location, turning off both");
		_root = "";
		_alias = "";
	}
}

void Locations::getLocationRegex(void)
{
	_locationDirective = extractValue("location");
	logger.log(INFO, "Found location: " + _locationDirective);
	setRoot();
	setIndex();
	setMethods();
	setReturn();
	setAlias();
	setAutoIndex();
	setClientBodySize();
	checkIfBothAliasAndRoot();
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

Locations::Locations(std::string locationContent): _locationContent(""), _methodsList(""),
	_locationDirective(""), _root(""), _index(""), _alias(""), _return(""), _autoindex(false)
{
	_locationContent = locationContent;
	_allowedMethods.GET = false;
	_allowedMethods.DELETE = false;
	_allowedMethods.POST = false;
	getLocationRegex();
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
	_root = extractValue(" root ");
	if (_root.empty())
		_root = extractValue("	root ");
	if (!_root.empty())
	{
		logger.log(DEBUG, "Root set as: " + _root);
	}
}

void Locations::setIndex(void)
{
	_index = extractValue("	index");
	if (!_index.empty())
	{
		logger.log(DEBUG, "Index set as: " + _index);
	}
}

void Locations::setReturn(void)
{
	_return =  extractValue("return");
	if (!_return.empty())
	{
		logger.log(DEBUG, "Return set as: " + _return);
	}

}

std::string Locations::getRoot(void)
{
	return (_root);
}
std::string Locations::getIndex(void)
{
	return (_index);
}


Locations::~Locations()
{
}