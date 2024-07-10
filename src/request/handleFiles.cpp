/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   handleFiles.cpp                                    :+:    :+:            */
/*                                                     +:+                    */
/*   By: rfinneru <rfinneru@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/07/09 15:04:20 by rfinneru      #+#    #+#                 */
/*   Updated: 2024/07/10 11:35:37 by ruben         ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Request.hpp"

void trimFirstChar(std::string &str)
{
	str = str.substr(1, str.size());	
}

void trimLastChar(std::string &str)
{
	str = str.substr(0, str.size()-1);	
}

void printFileStruct(file_t *file)
{
	std::cout << "fileName: " << file->fileName << std::endl;
    std::cout << "fileContent: " << file->fileContent << std::endl;
    std::cout << "fileContentType: " << file->fileContentType << std::endl;
    std::cout << "fileContentDisposition: " << file->fileContentDisposition << std::endl;
    std::cout << "fileBoundary: " << file->fileBoundary << std::endl;
}

void findFileContent(request_t *req, file_t *requestFile)
{
	
}

void setFile(request_t *req, file_t *requestFile)
{
	std::string cLength;
	std::string totalFileContent;

	cLength = trim(extractValue(req, "Content-Length: "));
	if (!cLength.empty())
		req->contentLength = std::stoi(cLength);
    else 
        req->contentLength = 0;
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