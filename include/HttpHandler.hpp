/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpHandler.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jade-haa <jade-haa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/13 20:00:39 by jade-haa          #+#    #+#             */
/*   Updated: 2024/06/14 14:50:29 by jade-haa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Webserv.hpp"
#include <iostream>

struct		MethodsReq
{
	bool	GET;
	bool	POST;
	bool	DELETE;
} typedef MethodsReq;

class Server;

class HttpHandler : public Webserv
{
  private:
	std::string _responseContent;
	std::string _requestContent;
	std::string _request;
	MethodsReq _allowedMethods;
	std::string _requestURL;

  public:
	HttpHandler();
	~HttpHandler();
	std::string extractValue(std::string toSearch);
	void setResponseContent(std::string content);
	void setRequest();
	void setMethods(void);

	void matchResponse(void);
	void handleRequest(const std::string &content);
	std::string getResponseContent(void);
	std::string setRequestContent(void);

	const std::vector<Server>& getServers() const;
};
