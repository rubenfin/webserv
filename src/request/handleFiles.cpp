/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   handleFiles.cpp                                    :+:    :+:            */
/*                                                     +:+                    */
/*   By: rfinneru <rfinneru@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/07/09 15:04:20 by rfinneru      #+#    #+#                 */
/*   Updated: 2024/07/18 14:21:34 by rfinneru      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Request.hpp"
#include <bits/stdc++.h>

void clearUntilDoubleNewline(std::string& str) {
    size_t pos = str.find("\r\n\r\n");
    if (pos != std::string::npos) {
        str.erase(0, pos + 4);  // +4 to keep the "\r\n\r\n"
    } else {
        str.clear();  // If "\r\n\r\n" is not found, clear the entire string
    }
}

void clearLastLine(std::string& str) {
    size_t lastNewline = str.find_last_of('\n');
    if (lastNewline != std::string::npos) {
        size_t secondLastNewline = str.find_last_of('\n', lastNewline - 1);
        if (secondLastNewline != std::string::npos) {
            str.erase(secondLastNewline + 1, lastNewline - secondLastNewline - 1);
        } else {
            str.erase(0, lastNewline);
        }
    }
}

void trimFirstChar(std::string &str)
{
	str = str.substr(1, str.size());	
}

void trimLastChar(std::string &str)
{
	str = str.substr(0, str.size() - 1);	
}

void printFileStruct(file_t *file) {

    logger.log(REQUEST, "fileName: " + file->fileName);
    logger.log(REQUEST, "fileContent: ");
    // for(int i = 0; i < file->fileContent.size(); i++)
    //     std::cout << file->fileContent[i] << std::endl;
    logger.log(REQUEST, "fileContentType: " + file->fileContentType);
    logger.log(REQUEST, "fileContentDisposition: " + file->fileContentDisposition);
    logger.log(REQUEST, "fileBoundary: " + file->fileBoundary);
}

void findFileContent(request_t *req, file_t *requestFile) {

    req->rawBody.erase(req->rawBody.begin(), req->rawBody.begin() + 4);

    auto headerEnd = std::search(req->rawBody.begin(), req->rawBody.end(),
                                 std::begin("\r\n\r\n"), std::end("\r\n\r\n") - 1);
    if (headerEnd == req->rawBody.end()) {
        requestFile->fileContent.clear();
        return;
    }
    auto start = headerEnd + 4;

    std::string endBoundary = "\r\n--" + requestFile->fileBoundary + "--";
    auto end = std::search(start, req->rawBody.end(),
                           endBoundary.begin(), endBoundary.end());

    if (end == req->rawBody.end()) {
        end = req->rawBody.end();
    }

    requestFile->fileContent.assign(start, end);
}


void setFile(request_t *req, file_t *requestFile)
{
	std::string cLength;
	std::string totalFileContent;

	cLength = trim(extractValue(req, "Content-Length: "));
	if (req->method == POST)
		req->contentLength = std::stoi(cLength);
    else 
	{
        req->contentLength = 0;
		logger.log(DEBUG, "Did not proceed with setFile(), contentLength = 0");
		return;
	}
	logger.log(DEBUG, "File exists set to true in RequestFile");
	requestFile->fileExists = true;
	requestFile->fileContentType = trim(extractValue(req, "Content-Type: "));
	trimLastChar(requestFile->fileContentType);
	if (requestFile->fileContentType == "multipart/form-data")
	requestFile->fileBoundary = trim(extractValue(req, "Content-Type: multipart/form-data; boundary="));
	if (!requestFile->fileBoundary.empty())
	{
		requestFile->fileContentDisposition = trim(extractValue(req, "Content-Disposition: "));
		trimLastChar(requestFile->fileContentDisposition);
		requestFile->fileName = trim(extractValue(req, "filename="));
		trimFirstChar(requestFile->fileName);
		trimLastChar(requestFile->fileName);
        for (unsigned char element : req->rawBody) {
        if (std::isprint(element)) {
            std::cout << element;
        } else {
            std::cout << "\\x" << std::hex << static_cast<int>(element);
        }
        }
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;

		findFileContent(req, &req->file);
	}

    for (unsigned char element : req->file.fileContent) {
        if (std::isprint(element)) {
            std::cout << element;
        } else {
            std::cout << "\\x" << std::hex << static_cast<int>(element);
        }
    }
    std::cout << std::endl;
}