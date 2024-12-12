/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   parseRequest.cpp                                   :+:    :+:            */
/*                                                     +:+                    */
/*   By: rfinneru <rfinneru@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/24 16:12:04 by rfinneru      #+#    #+#                 */
/*   Updated: 2024/12/03 16:38:27 by rfinneru      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Request.hpp"

void replaceEncodedSpaces(std::string& text) {
    size_t newlinePos = text.find('\n');
    std::string firstLine = (newlinePos != std::string::npos) ? text.substr(0, newlinePos) : text;

    size_t pos = firstLine.find("%20");
	if (pos == std::string::npos)
		return;

    while (pos != std::string::npos) { 
        firstLine.replace(pos, 3, " ");
        pos = firstLine.find("%20", pos + 1);
    }

    text = firstLine + text.substr(newlinePos); 
}

std::string extractValueDoubleQuote(request_t &req, const std::string &toSearch) {
    std::size_t keywordPos = req.requestContent.find(toSearch);
    if (keywordPos != std::string::npos) {
        std::size_t beginLocation = keywordPos + toSearch.size();

        if (beginLocation >= req.requestContent.size()) {
            return "";
        }

        while (beginLocation < req.requestContent.size() && std::isspace(req.requestContent[beginLocation])) {
            ++beginLocation;
        }

        std::size_t endLocation = beginLocation;
        while (endLocation < req.requestContent.size() && req.requestContent[endLocation] != '"') {
            ++endLocation;
        }

        if (endLocation > beginLocation) {
            return req.requestContent.substr(beginLocation, endLocation - beginLocation);
        }
    }
    return "";
}



std::string extractValue(request_t &req, const std::string &toSearch)
{
	std::size_t keywordPos = req.requestContent.find(toSearch);
	if (keywordPos != std::string::npos)
	{
		std::size_t beginLocation = keywordPos + toSearch.size();
		while (beginLocation < req.requestContent.size()
			&& std::isspace(req.requestContent[beginLocation]))
			++beginLocation;
		std::size_t endLocation = beginLocation;
		while (endLocation < req.requestContent.size()
			&& !std::isspace(req.requestContent[endLocation]))
			++endLocation;
		std::string directory = req.requestContent.substr(beginLocation,
				endLocation - beginLocation);
		return (directory);
	}
	return ("");
}

static void	setHttpVersion(request_t &req)
{
	std::size_t firstSpace = req.firstLine.find(" ");
	if (firstSpace == std::string::npos)
	{
		req.http_v = "invalid";
		return ;
	}
	std::size_t secondSpace = req.firstLine.find(" ", firstSpace + 1);
	if (secondSpace == std::string::npos)
	{
		req.http_v = "invalid";
		return ;
	}
	std::string httpVersion = req.firstLine.substr(secondSpace + 1);
	httpVersion = trim(httpVersion);
	if (httpVersion == "HTTP/1.1")
	{
		req.http_v = "1.1";
	}
	else
	{
		req.http_v = "invalid";
	}
}

static void	setMethod(request_t &req)
{
	req.firstLine.insert(0, 1, ' ');
	if (req.firstLine.find(" GET ") != std::string::npos)
		req.method = GET;
	else if (req.firstLine.find(" POST ") != std::string::npos)
		req.method = POST;
	else if (req.firstLine.find(" DELETE ") != std::string::npos)
		req.method = DELETE;
	else
		req.method = ERROR;
	req.firstLine = trim(req.firstLine);
}

static void setRequestURL(request_t &req)
{
	std::size_t startPos = req.firstLine.find(" ");
	if (startPos == std::string::npos)
	{
		req.requestURL = "";
		return ;
	}
	if (req.firstLine.find("/favicon.ico") != std::string::npos)
	{
		logger.log(DEBUG, "Found a /favicon.ico send back 404");
		throw FavIconException();
	}
	startPos = req.firstLine.find("/", startPos);
	if (startPos == std::string::npos)
	{
		req.requestURL = "";
		return ;
	}
	std::size_t endPos = req.firstLine.find(" ", startPos);
	if (endPos == std::string::npos)
		req.requestURL = req.firstLine.substr(startPos);
	else
		req.requestURL = req.firstLine.substr(startPos, endPos - startPos);
	if(!req.requestURL.empty())
		replaceEncodedSpaces(req.requestURL);
}

static void setRequestDirFile(request_t &req)
{
	if (req.requestURL.find(".") == std::string::npos)
	{
		req.requestDirectory = req.requestURL;
	}
	else
	{
		auto lastSlash = req.requestURL.rfind("/");
		if (lastSlash != std::string::npos)
		{
			req.requestFile = req.requestURL.substr(lastSlash);
			req.requestDirectory = req.requestURL.substr(0, lastSlash);
		}
	}
	if (!req.requestDirectory.empty())
		replaceEncodedSpaces(req.requestDirectory);
}

static void	setRequestHeader(request_t &req)
{
	std::size_t startPos = req.requestContent.find("\n") + 1;
	std::size_t endPos;
	while ((endPos = req.requestContent.find("\n", startPos)) != std::string::npos)
	{
		std::string headerLine = req.requestContent.substr(startPos, endPos
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
		req.header[key] = value;
		startPos = endPos + 1;
	}
}

static void	setRequestBody(request_t &req)
{
	std::size_t foundBody = req.requestContent.find("\r\n\r\n");
	if (foundBody == std::string::npos)
	{
		req.requestBody = "";
		return ;
	}
	foundBody += 4;
	req.requestBody = req.requestContent.substr(foundBody);
}

static void findHeadingEnd(request_t &req)
{
	std::size_t foundBody = req.requestContent.find("\r\n\r\n");
	if (foundBody != std::string::npos)
	{
		req.foundHeaderEnd = true;
	}
}

static void setCookie(request_t& req)
{
	auto it = req.header.find("Cookie");
	if (it != req.header.end())
	{
		req.cookie = it->second;
	}
	else
		return ;
	std::size_t equal = req.cookie.find("id=") + 3;
	if (equal == std::string::npos)
	{
		req.cookie = "";
		return ;
	}
	req.cookie = req.cookie.substr(equal, req.cookie.size() - equal - 1);
	trim(req.cookie);
}

void	parse_request(request_t &req, std::string buffer)
{
	try
	{
		req.requestContent = buffer;
		req.file.fileExists = false;
		req.firstLine = req.requestContent.substr(0, req.requestContent.find("\n"));
		findHeadingEnd(req);
		setHttpVersion(req);
		setRequestURL(req);
		setMethod(req);
		setRequestDirFile(req);
		setRequestHeader(req);
		setRequestBody(req);
		setCookie(req);
		setFile(req, &req.file);
		printRequestStruct(req);
		if (req.contentLength)
			printFileStruct(&req.file);
	}
	catch (const FavIconException &e)
	{
		req.favIcon = true;
	}
	catch(const std::exception& e)
	{
		logger.log(ERR, std::string(e.what()));
		req.internalError = true;
	}
}