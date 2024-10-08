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
	Server* 	_server;
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
	long long 	_client_body_size;
	bool 		_autoindex;

  public:
	void linkServer(Server *server);
	void printMethods(void);
	std::string getLocationDirective(void);
	void setRoot(void);
	void setIndex(void);
	void setCgi_pass(void);
	void setMethods(void);
	void setAlias(void);
	void setReturn(void);
	void setAutoIndex(void);
	void setClientBodySize(void);
	std::string getRoot(void);
	std::string getIndex(void);
	std::string getCgi_pass(void);
	MethodsLoc getMethods(void);
	std::string getAlias(void);
	std::string getReturn(void);
	bool 		getAutoIndex(void);
	long long  	getClientBodySize(void);
	std::string extractValue(std::string toSearch);
	std::string extractValueUntilNL(std::string toSearch);
	void printLocationsContent(void);
	void getLocationRegex(void);
	Locations(std::string locationContent);
	Locations(const Locations *other);
	Locations();
	~Locations();
};
