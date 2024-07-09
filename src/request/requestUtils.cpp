/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   requestUtils.cpp                                   :+:    :+:            */
/*                                                     +:+                    */
/*   By: rfinneru <rfinneru@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/24 18:36:00 by rfinneru      #+#    #+#                 */
/*   Updated: 2024/07/09 15:10:42 by rfinneru      ########   odam.nl         */
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

void	printRequestStruct(request_t *req)
{
    std::cout << BLUE << "PARSED REQUEST" <<std::endl;
    std::cout << "----------------------------------" << std::endl;
	// std::cout << "requestContent: " << r->requestContent << std::endl;
	std::cout << "http_v: " << req->http_v << std::endl;
	std::cout << "firstLine: " << req->firstLine << std::endl;
	std::cout << "requestBody: " << req->requestBody << std::endl;
	std::cout << "requestURL: " << req->requestURL << std::endl;
	std::cout << "method: " << req->method << std::endl;
	std::cout << "requestDirectory: " << req->requestDirectory << std::endl;
	std::cout << "contentLength: " << req->contentLength << std::endl;
	std::cout << "contentType: " << req->contentType << std::endl;
	// std::cout << "_header: " << std::endl;
	// for (const auto &header : r->header)
	// {
	// 	std::cout << header.first << ": " << header.second << std::endl;
	// }
    std::cout << "\nEND PR ---------------------------------- END PR" << std::endl;
    std::cout << RESET << std::endl;;
}
