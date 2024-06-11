/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Locations.hpp                                      :+:    :+:            */
/*                                                     +:+                    */
/*   By: jade-haa <jade-haa@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/09 16:11:43 by jade-haa      #+#    #+#                 */
/*   Updated: 2024/06/11 17:11:26 by rfinneru      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once
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
	std::string _url;
	std::string _locationContent;
	std::string _root;
	std::string _index;
	std::string _cgi_pass;
	

  public:
	std::string extractValue(const std::string &searchString);
	void printLocationsContent(void);
	std::string extractDirectory(const std::string &line);
	std::vector<std::string> getLocationRegex(std::string locationContent);
	Locations(std::string locationContent);
	~Locations();
};
