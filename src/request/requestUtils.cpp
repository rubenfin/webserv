/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   requestUtils.cpp                                   :+:    :+:            */
/*                                                     +:+                    */
/*   By: rfinneru <rfinneru@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/24 18:36:00 by rfinneru      #+#    #+#                 */
/*   Updated: 2024/06/25 15:55:27 by rfinneru      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Request.hpp"

std::string trim(const std::string &str)
{
	std::string::const_iterator left = std::find_if(str.begin(), str.end(),
			[](unsigned char ch) { return (!std::isspace(ch)); });
	std::string::const_iterator right = std::find_if(str.rbegin(), str.rend(),
			[](unsigned char ch) { return (!std::isspace(ch)); }).base();
	return (right <= left ? std::string() : std::string(left, right));
}

void	resetRequest(request_t &request)
{
	request.http_v = "";
	request.firstLine = "";
	request.requestContent = "";
	request.requestBody = "";
	request.requestURL = "";
	request.requestDirectory = "";
	request.requestFile = "";
}

void	printRequestStruct(request_t *r)
{
    std::cout << BLUE << "REQUEST" <<std::endl;
    std::cout << "----------------------------------" << std::endl;
	std::cout << "http_v: " << r->http_v << std::endl;
	std::cout << "firstLine: " << r->firstLine << std::endl;
	std::cout << "requestContent: " << r->requestContent << std::endl;
	std::cout << "requestBody: " << r->requestBody << std::endl;
	std::cout << "requestURL: " << r->requestURL << std::endl;
	std::cout << "method: " << r->method << std::endl;
	std::cout << "requestDirectory: " << r->requestDirectory << std::endl;
	std::cout << "requestFile: " << r->requestFile << std::endl;
	// std::cout << "_header: " << std::endl;
	// for (const auto &header : r->header)
	// {
	// 	std::cout << header.first << ": " << header.second << std::endl;
	// }
    std::cout << "----------------------------------" << std::endl;
    std::cout << RESET << std::endl;;
}
