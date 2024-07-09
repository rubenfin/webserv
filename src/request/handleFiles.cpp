/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   handleFiles.cpp                                    :+:    :+:            */
/*                                                     +:+                    */
/*   By: rfinneru <rfinneru@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/07/09 15:04:20 by rfinneru      #+#    #+#                 */
/*   Updated: 2024/07/09 15:54:57 by rfinneru      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Request.hpp"

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
	requestFile->fileContentType = requestFile->fileContentType.substr(0, requestFile->fileContentType.size()-1);
	if (requestFile->fileContentType == "multipart/form-data")
	requestFile->fileBoundary = trim(extractValue(req, "Content-Type: multipart/form-data; boundary="));
	if (!requestFile->fileBoundary.empty())
	{
		
	}
}