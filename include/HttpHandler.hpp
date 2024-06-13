/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpHandler.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jade-haa <jade-haa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/13 20:00:39 by jade-haa          #+#    #+#             */
/*   Updated: 2024/06/13 21:24:43 by jade-haa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Webserv.hpp"
#include <iostream>

class HttpHandler
{
  private:
	std::string _responseContent;
	std::string _requestContent;
	std::string _request;

  public:
	HttpHandler();
	~HttpHandler();
	std::string extractValue(std::string toSearch);
	void setResponseContent(std::string content);
	void getRequest();
	void handleRequest(const std::string &content);
	std::string getResponseContent(void);
	std::string getRequestContent(void);
};
