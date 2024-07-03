/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   HttpHandler.hpp                                    :+:    :+:            */
/*                                                     +:+                    */
/*   By: jade-haa <jade-haa@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/13 20:00:39 by jade-haa      #+#    #+#                 */
/*   Updated: 2024/07/03 12:49:10 by rfinneru      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "HttpStatusCodes.hpp"
#include "Webserv.hpp"
#include <iostream>
#include <string>

class				Locations;
class				Server;
struct				request_t;
struct				response_t;

class HttpHandler
{
  private:
	request_t		*_request;
	response_t 		*_response;
	Locations		*_foundDirective;
	Server			*_server;
	bool			_isCgi;
	bool			_hasRedirect;

  public:
	HttpHandler();
	~HttpHandler();
	
	void handleRequestBody(void);
	void checkRequestData(void);
	void combineRightUrl(void);
	void handleRequest(Server &serverAddresss, request_t *request, response_t *response);
	void httpVersionCheck(void);
	int pathCheck(void);
	void methodCheck(void);
	Locations	*getFoundDirective(void);
	request_t	*getRequest(void);
	response_t	*getResponse(void);
	Server		*getServer(void);
	Locations	*findMatchingDirective(void);
	bool		getCgi(void);
	bool		getRedirect(void);
};
