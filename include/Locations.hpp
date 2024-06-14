/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Locations.hpp                                      :+:    :+:            */
/*                                                     +:+                    */
/*   By: jade-haa <jade-haa@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/09 16:11:43 by jade-haa      #+#    #+#                 */
/*   Updated: 2024/06/14 16:37:12 by rfinneru      ########   odam.nl         */
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

struct			MethodsLoc
{
	bool		GET;
	bool		POST;
	bool		PUT;
	bool		DELETE;
	bool		PATCH;
} typedef MethodsLoc;

class Locations
{
  private:
	std::string _locationContent;
	std::string _methodsList;

	std::string _locationDirectory;
	MethodsLoc	_allowedMethods;
	std::string _root;
	std::string _index;
	std::string _cgi_pass;

	std::string _url;

  public:
	void printMethods(void);
	std::string getLocationDirectory(void);
	void setRoot(void);
	void setIndex(void);
	void setCgi_pass(void);
	void setMethods(void);

	std::string getRoot(void);
	std::string getIndex(void);
	std::string getCgi_pass(void);
	std::string extractValue(std::string toSearch);
	void printLocationsContent(void);
	std::string extractDirectory(const std::string &line);
	void getLocationRegex(std::string locationContent);
	Locations(std::string locationContent);
	~Locations();
};
