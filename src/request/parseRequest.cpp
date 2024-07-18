/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   parseRequest.cpp                                   :+:    :+:            */
/*                                                     +:+                    */
/*   By: rfinneru <rfinneru@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/24 16:12:04 by rfinneru      #+#    #+#                 */
/*   Updated: 2024/07/18 16:04:01 by rfinneru      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Request.hpp"

std::string extractValue(request_t *req, const std::string &toSearch)
{
	std::size_t keywordPos = req->requestContent.find(toSearch);
	if (keywordPos != std::string::npos)
	{
		std::size_t beginLocation = keywordPos + toSearch.size();
		while (beginLocation < req->requestContent.size()
			&& std::isspace(req->requestContent[beginLocation]))
			++beginLocation;
		std::size_t endLocation = beginLocation;
		while (endLocation < req->requestContent.size()
			&& !std::isspace(req->requestContent[endLocation]))
			++endLocation;
		std::string directory = req->requestContent.substr(beginLocation,
				endLocation - beginLocation);
		return (directory);
	}
	return ("");
}

static void	setHttpVersion(request_t *req)
{
	std::size_t firstSpace = req->firstLine.find(" ");
	if (firstSpace == std::string::npos)
	{
		req->http_v = "invalid";
		return ;
	}
	std::size_t secondSpace = req->firstLine.find(" ", firstSpace + 1);
	if (secondSpace == std::string::npos)
	{
		req->http_v = "invalid";
		return ;
	}
	std::string httpVersion = req->firstLine.substr(secondSpace + 1);
	httpVersion = trim(httpVersion);
	if (httpVersion == "HTTP/1.1")
	{
		req->http_v = "1.1";
	}
	else
	{
		req->http_v = "invalid";
	}
}

static void	setMethod(request_t *req)
{
	if (req->firstLine.find("GET") != std::string::npos)
		req->method = GET;
	else if (req->firstLine.find("POST") != std::string::npos)
		req->method = POST;
	else if (req->firstLine.find("DELETE") != std::string::npos)
		req->method = DELETE;
	else
		req->method = ERROR;
	// TODO: Throw an exception if no method found?
}

static void	setRequestURL(request_t *req)
{
	std::size_t startPos = req->firstLine.find(" ");
	if (startPos == std::string::npos)
	{
		req->requestURL = "";
		return ;
	}
	startPos = req->firstLine.find("/", startPos);
	if (startPos == std::string::npos)
	{
		req->requestURL = "";
		return ;
	}
	std::size_t endPos = req->firstLine.find(" ", startPos);
	if (endPos == std::string::npos)
		req->requestURL = req->firstLine.substr(startPos);
	else
		req->requestURL = req->firstLine.substr(startPos, endPos - startPos);
}

static void setRequestDirFile(request_t *req)
{
	if (req->requestURL.find(".") == std::string::npos)
		req->requestDirectory = req->requestURL;
	else
	{
		auto lastSlash = req->requestURL.rfind("/");
		if (lastSlash != std::string::npos)
		{
			req->requestFile = req->requestURL.substr(lastSlash);
			req->requestDirectory = req->requestURL.substr(0, lastSlash);
		}
	}
}

static void	setRequestHeader(request_t *req)
{
	std::size_t startPos = req->requestContent.find("\n") + 1;
	std::size_t endPos;
	while ((endPos = req->requestContent.find("\n",
				startPos)) != std::string::npos)
	{
		std::string headerLine = req->requestContent.substr(startPos, endPos
				- startPos);
		if (headerLine.find(":") == std::string::npos)
		{
			break ;
		}
		std::size_t delimiterPos = headerLine.find(":");
		std::string key = headerLine.substr(0, delimiterPos);
		std::string value = headerLine.substr(delimiterPos + 1);
		key.erase(0, key.find_first_not_of(" \t"));
		key.erase(key.find_last_not_of(" \t") + 1);
		value.erase(0, value.find_first_not_of(" \t"));
		value.erase(value.find_last_not_of(" \t") + 1);
		req->header[key] = value;
		startPos = endPos + 1;
	}
}

static void	setRequestBody(request_t *req)
{
	std::size_t foundBody = req->requestContent.find("\r\n\r\n");
	if (foundBody == std::string::npos)
	{
		req->requestBody = "";
		return ;
	}
	foundBody += 4;
	req->requestBody = req->requestContent.substr(foundBody);
}

void setRawData(request_t *req, const char *buffer, ssize_t bufferSize) {

    req->rawData.assign(buffer, buffer + bufferSize);

    auto it = std::search(req->rawData.begin(), req->rawData.end(),
                          std::begin("\r\n\r\n"), std::end("\r\n\r\n") - 1);

    if (it == req->rawData.end()) {
        req->rawBody.clear();
        return;
    }

    req->rawBody.assign(it, req->rawData.end());

    // Print the raw body
    // std::cout << "Raw body contents:" << std::endl;
    // for (unsigned char element : req->rawBody) {
    //     if (std::isprint(element)) {
    //         std::cout << element;
    //     } else {
    //         std::cout << "\\x" << std::hex << static_cast<int>(element);
    //     }
    // }
    // std::cout << std::endl;
}

void	parse_request(request_t *req, char *buffer, ssize_t bufferSize)
{
	setRawData(req, buffer, bufferSize);
	req->requestContent = buffer;
	req->file.fileExists = false;
	req->firstLine = req->requestContent.substr(0, req->requestContent.find("\n"));
	setHttpVersion(req);
	setMethod(req);
	setRequestURL(req);
	setRequestDirFile(req);
	setRequestHeader(req);
	setRequestBody(req);
	setFile(req, &req->file);
	printRequestStruct(req);
	if (req->contentLength)
		printFileStruct(&req->file);
}