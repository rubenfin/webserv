/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Server.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: jade-haa <jade-haa@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/09 15:40:25 by jade-haa      #+#    #+#                 */
/*   Updated: 2024/06/12 13:25:52 by rfinneru      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Locations.hpp"
#include <iostream>
#include <regex>
#include <sstream>
#include <string>
#include <unordered_set>

enum class HttpMethod
{
	GET,
	POST,
	PUT,
	DELETE,
	PATCH
};

class Locations;

class Server
{
  protected:
	int _socketfd;
	std::string _serverContent;
	std::string _serverName;
	std::string _portString;
	u_int16_t _port;
	std::string _root;
	std::string _index;
	std::string _methodsList;
	std::unordered_set<HttpMethod> _allowMethods;
	std::vector<Locations> _locations;

  public:
	void getLocationStack(std::string locationContent);
	std::string extractValue(const std::string &searchString);
	void setServerName(void);
	void setPort(void);
	void setRoot(void);
	void setIndex(void);
	void setMethods(void);
	void setSockedFD(int fd);
	std::string getServerName(void);
	std::string getPortString(void);
	u_int16_t getPort(void);
	std::string getRoot(void);
	std::string getIndex(void);
	int getSocketFD(void);
	std::unordered_set<HttpMethod> getMethods(void);
	std::string getMethodsList(void);
	std::vector<Locations> getLocation(void);
	void setLocationsRegex(std::string serverContent);
	Server(std::string serverContent);
};