/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Locations.hpp                                      :+:    :+:            */
/*                                                     +:+                    */
/*   By: jade-haa <jade-haa@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/09 16:11:43 by jade-haa      #+#    #+#                 */
/*   Updated: 2024/07/08 16:15:10 by rfinneru      ########   odam.nl         */
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
	bool		DELETE;
} typedef MethodsLoc;

class Locations
{
  private:
	std::string _locationContent;
	std::string _methodsList;
	std::string _locationDirective;
	MethodsLoc	_allowedMethods;
	std::string _root;
	std::string _index;
	std::string _cgi_pass;
	std::string _alias;
	std::string _url;
	std::string _return;

  public:
	void printMethods(void);
	std::string getLocationDirective(void);
	void setRoot(void);
	void setIndex(void);
	void setCgi_pass(void);
	void setMethods(void);
	void setAlias(void);
	void setReturn(void);

	std::string getRoot(void);
	std::string getIndex(void);
	std::string getCgi_pass(void);
	MethodsLoc getMethods(void);
	std::string getAlias(void);
	std::string getReturn(void);
	std::string extractValue(std::string toSearch);
	std::string extractValueUntilNL(std::string toSearch);
	void printLocationsContent(void);
	void getLocationRegex(std::string locationContent);
	Locations(std::string locationContent);
	Locations(const Locations *other);
	Locations();
	~Locations();
};
