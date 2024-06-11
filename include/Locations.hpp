/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Locations.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jade-haa <jade-haa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/09 16:11:43 by jade-haa          #+#    #+#             */
/*   Updated: 2024/06/11 15:58:35 by jade-haa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Webserv.hpp"
#include <iostream>
#include <regex>
#include <sstream>
#include <string>
#include <unordered_set>

class Locations
{
  private:
	std::string url_;
	std::string locationContent_;
	std::string root_;
	std::string index_;
	std::string cgi_pass;
	

  public:
	std::string extractValue(const std::string &searchString)
	{
		size_t pos = locationContent_.find(searchString);
		if (pos != std::string::npos)
		{
			pos += searchString.length();
			size_t endPos = locationContent_.find('\n', pos);
			if (endPos != std::string::npos)
				return (locationContent_.substr(pos, endPos - pos));
			else
				return (locationContent_.substr(pos));
		}
		return ("");
	}

	void printLocationsContent(void)
	{
		std::cout << locationContent_ << std::endl;
	}
	std::string extractDirectory(const std::string &line)
	{
		std::string keyword = "location";
		std::size_t keywordPos = line.find(keyword);
		if (keywordPos != std::string::npos)
		{
			std::size_t beginLocation = keywordPos + keyword.size();

			while (beginLocation < line.size()
				&& std::isspace(line[beginLocation]))
				++beginLocation;

			std::size_t endLocation = beginLocation;
			while (endLocation < line.size()
				&& !std::isspace(line[endLocation]))
				++endLocation;

			std::string directory = line.substr(beginLocation, endLocation
					- beginLocation);

			return (directory);
		}
		return ("");
	}
	std::vector<std::string> getLocationRegex(std::string locationContent)
	{
		std::string line;
		std::regex startPattern(R"(location)");
		std::istringstream iss(locationContent);
		std::vector<std::string> result;
		int index = 0;

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
	Locations(std::string locationContent)
	{
		this->url_ = "test";
		locationContent_ = locationContent;
		// std::cout << locationContent << std::endl;
		// std::vector<std::string> content = getLocationStack(locationContent);
		std::vector<std::string> content = getLocationRegex(locationContent);

		// for (const std::string &line : content)
		// {
		// 	std::cout << "Location: " << line << std::endl;
		// }
	}
	~Locations()
	{
	}
};
