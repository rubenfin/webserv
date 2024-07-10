/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   handleFiles.cpp                                    :+:    :+:            */
/*                                                     +:+                    */
/*   By: rfinneru <rfinneru@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/07/09 15:04:20 by rfinneru      #+#    #+#                 */
/*   Updated: 2024/07/10 16:29:02 by rfinneru      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Request.hpp"

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
	str = str.substr(0, str.size()-1);	
}

void printFileStruct(file_t *file) {
    Logger& logger = Logger::getInstance();

    logger.log(REQUEST, "fileName: " + file->fileName);
    logger.log(REQUEST, "fileContent: " + file->fileContent);
    logger.log(REQUEST, "fileContentType: " + file->fileContentType);
    logger.log(REQUEST, "fileContentDisposition: " + file->fileContentDisposition);
    logger.log(REQUEST, "fileBoundary: " + file->fileBoundary);
}

void findFileContent(request_t *req, file_t *requestFile)
{
	std::size_t start = req->requestContent.find("\r\n\r\n");
	std::size_t end = req->requestContent.find(requestFile->fileBoundary + "--");
	requestFile->fileContent = req->requestContent.substr(start, end);
	// clearLastLine(requestFile->fileContent);
	// clearUntilDoubleNewline(requestFile->fileContent);
}

void setFile(request_t *req, file_t *requestFile)
{
	std::string cLength;
	std::string totalFileContent;
	Logger& logger = Logger::getInstance();

	cLength = trim(extractValue(req, "Content-Length: "));
	if (!cLength.empty())
		req->contentLength = std::stoi(cLength);
    else 
	{
        req->contentLength = 0;
		logger.log(WARNING, "Did not proceed with setFile(), contentLength = 0");
		return;
	}
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
		findFileContent(req, &req->file);
	}
}