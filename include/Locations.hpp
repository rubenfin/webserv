/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Locations.hpp                                      :+:    :+:            */
/*                                                     +:+                    */
/*   By: jade-haa <jade-haa@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/09 16:11:43 by jade-haa      #+#    #+#                 */
/*   Updated: 2024/08/19 14:26:23 by rfinneru      ########   odam.nl         */
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

class Server;

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
	std::string _alias;
	std::string _return;
	long long 	_client_body_size;
	bool 		_autoindex;

  public:
	Locations(std::string locationContent);
	~Locations();
	
	void printMethods(void);
	std::string getLocationDirective(void);
	void setRoot(void);
	void setIndex(void);
	void setMethods(void);
	void setAlias(void);
	void setReturn(void);
	void setAutoIndex(void);
	void setClientBodySize(void);
	std::string getRoot(void);
	std::string getIndex(void);
	MethodsLoc getMethods(void);
	std::string getAlias(void);
	std::string getReturn(void);
	bool 		getAutoIndex(void);
	long long  	getClientBodySize(void);
	void checkIfBothAliasAndRoot();
	std::string extractValue(std::string toSearch);
	std::string extractValueUntilNL(std::string toSearch);
	void printLocationsContent(void);
	void getLocationRegex(void);
};
