/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Locations.cpp                                      :+:    :+:            */
/*                                                     +:+                    */
/*   By: jade-haa <jade-haa@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/11 17:08:48 by rfinneru      #+#    #+#                 */
/*   Updated: 2024/06/17 11:31:34 by rfinneru      ########   odam.nl         */
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
	std::cout << _locationContent << std::endl;
}
void Locations::setMethods(void)
{
	_methodsList = extractValue("methods");
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

void Locations::getLocationRegex(std::string locationContent)
{
	_locationDirectory = extractValue("location");
	_methodsList = extractValue("allow_methods");
	setRoot();
	setIndex();
	setCgi_pass();
	setMethods();
}

std::string Locations::getLocationDirectory(void)
{
	return (_locationDirectory);
}

Locations::Locations(std::string locationContent)
{
	this->_url = "test";
	_locationContent = locationContent;
	// std::cout << locationContent << std::endl;
	// std::vector<std::string> content = getLocationStack(locationContent);
	getLocationRegex(locationContent);
	// for (const std::string &line : content)
	// {
	// 	std::cout << "Location: " << line << std::endl;
	// }
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
}
void Locations::setIndex(void)
{
	_index = extractValue("index");
}
void Locations::setCgi_pass(void)
{
	_cgi_pass = extractValue("cgi_pass");
}

std::string Locations::getRoot(void)
{
	return(_root);
}
std::string Locations::getIndex(void)
{
	return(_index);
}
std::string Locations::getCgi_pass(void)
{
	return(_cgi_pass);
}

Locations::~Locations()
{
}