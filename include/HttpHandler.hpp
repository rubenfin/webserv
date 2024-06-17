/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   HttpHandler.hpp                                    :+:    :+:            */
/*                                                     +:+                    */
/*   By: jade-haa <jade-haa@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/13 20:00:39 by jade-haa      #+#    #+#                 */
/*   Updated: 2024/06/17 13:28:11 by rfinneru      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Webserv.hpp"
#include <iostream>

struct			MethodsReq
{
	bool		GET;
	bool		POST;
	bool		DELETE;
} typedef MethodsReq;

class			Server;

class HttpHandler
{
  private:
	std::string _responseContent;
	std::string _requestContent;
	std::string _request;
	std::string _response;
	MethodsReq	_allowedMethods;
	std::string _requestURL;
	Server		*_server;

  public:
	HttpHandler();
	~HttpHandler();
	std::string extractValue(const std::string &toSearch);
	void setResponseContent(std::string content);
	void setRequest();
	void setMethods(void);

	void matchResponse(void);
	void handleRequest(const std::string &content, Server &serverAddress);
	std::string getResponseContent(void);
	std::string getResponseURL(void);
	std::string setRequestContent(void);
	std::string findRequestedURL(const std::string &content);
	// const std::vector<Server>& getServers() const;
};
