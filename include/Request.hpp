/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Request.hpp                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: rfinneru <rfinneru@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/24 17:36:08 by rfinneru      #+#    #+#                 */
/*   Updated: 2024/11/11 13:40:54 by rfinneru      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Webserv.hpp"
#include <map>
#include <string.h>
#include <string>
#include <sys/stat.h>
#include <vector>
#include <sys/types.h>
#include "HTTPHandler.hpp"
#include "File.hpp"

struct request_t;

void		setFile(request_t &req, file_t *requestFile);
void		parse_request(request_t &req, std::string buffer);
void		resetRequest(request_t &request);
void		printRequestStruct(request_t &req);
std::string trim(const std::string &str);
std::string extractValue(request_t &req, const std::string &toSearch);
std::string extractValueDoubleQuote(request_t &req, const std::string &toSearch);
