/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Locations.cpp                                      :+:    :+:            */
/*                                                     +:+                    */
/*   By: rfinneru <rfinneru@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/11 17:08:48 by rfinneru      #+#    #+#                 */
/*   Updated: 2024/06/11 17:09:39 by rfinneru      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Locations.hpp"

std::string Locations::extractValue(const std::string &searchString)
{
	size_t	pos;
	size_t	endPos;

	pos = _locationContent.find(searchString);
	if (pos != std::string::npos)
	{
		pos += searchString.length();
		endPos = _locationContent.find('\n', pos);
		if (endPos != std::string::npos)
			return (_locationContent.substr(pos, endPos - pos));
		else
			return (_locationContent.substr(pos));
	}
	return ("");
}


void Locations::printLocationsContent(void)
{
	std::cout << _locationContent << std::endl;
}

std::string Locations::extractDirectory(const std::string &line)
{
	std::string keyword = "location";
	std::size_t keywordPos = line.find(keyword);
	if (keywordPos != std::string::npos)
	{
		std::size_t beginLocation = keywordPos + keyword.size();
		while (beginLocation < line.size() && std::isspace(line[beginLocation]))
			++beginLocation;
		std::size_t endLocation = beginLocation;
		while (endLocation < line.size() && !std::isspace(line[endLocation]))
			++endLocation;
		std::string directory = line.substr(beginLocation, endLocation
				- beginLocation);
		return (directory);
	}
	return ("");
}

std::vector<std::string> Locations::getLocationRegex(std::string locationContent)
{
	int	index;

	std::string line;
	std::regex startPattern(R"(location)");
	std::istringstream iss(locationContent);
	std::vector<std::string> result;
	index = 0;
	while (std::getline(iss, line))
	{
		if (std::regex_search(line, startPattern))
		{
			result.push_back(extractDirectory(line));
			// std::cout << "directory" << result[index] << std::endl;
			index++;
		}
	}
	return (result);
}

Locations::Locations(std::string locationContent)
{
	this->_url = "test";
	_locationContent = locationContent;
	// std::cout << locationContent << std::endl;
	// std::vector<std::string> content = getLocationStack(locationContent);
	std::vector<std::string> content = getLocationRegex(locationContent);
	// for (const std::string &line : content)
	// {
	// 	std::cout << "Location: " << line << std::endl;
	// }
}

Locations::~Locations()
{
}