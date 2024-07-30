/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   requestUtils.cpp                                   :+:    :+:            */
/*                                                     +:+                    */
/*   By: rfinneru <rfinneru@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/24 18:36:00 by rfinneru      #+#    #+#                 */
/*   Updated: 2024/07/30 16:14:20 by rfinneru      ########   odam.nl         */
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
	request.file.fileEndBoundaryFound = false;
}

void	printRequestStruct(request_t *req)
{
	logger.log(REQUEST, "PARSED REQUEST");
	logger.log(REQUEST, "----------------------------------");
	logger.log(REQUEST, "http_v: " + req->http_v);
	logger.log(REQUEST, "firstLine: " + req->firstLine);
	logger.log(REQUEST, "requestBody: " + req->requestBody);
    logger.log(DEBUG, 	"FILESIZE OF REQUESTBODY: " + std::to_string(req->requestBody.size()));
	logger.log(REQUEST, "requestURL: " + req->requestURL);
	logger.log(REQUEST, "requestDirectory: " + req->requestDirectory);
	logger.log(REQUEST, "requestFile: " + req->requestFile);
	logger.log(REQUEST, "contentLength: " + std::to_string(req->contentLength));
	logger.log(REQUEST, "contentType: " + req->contentType);
	// for (const auto &header : req->header) {
	//     logger.log(REQUEST, header.first + ": " + header.second);
	// }
	logger.log(REQUEST, "END PR ---------------------------------- END PR");
}
