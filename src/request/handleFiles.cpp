/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   handleFiles.cpp                                    :+:    :+:            */
/*                                                     +:+                    */
/*   By: rfinneru <rfinneru@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/07/09 15:04:20 by rfinneru      #+#    #+#                 */
/*   Updated: 2024/08/14 15:10:22 by rfinneru      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Request.hpp"
#include <bits/stdc++.h>

void	clearUntilDoubleNewline(std::string &str)
{
	size_t	pos;

	pos = str.find("\r\n\r\n");
	if (pos != std::string::npos)
	{
		str.erase(0, pos + 4); // +4 to keep the "\r\n\r\n"
	}
	else
	{
		str.clear(); // If "\r\n\r\n" is not found, clear the entire string
	}
}

void	clearLastLine(std::string &str)
{
	size_t	lastNewline;
	size_t	secondLastNewline;

	lastNewline = str.find_last_of('\n');
	if (lastNewline != std::string::npos)
	{
		secondLastNewline = str.find_last_of('\n', lastNewline - 1);
		if (secondLastNewline != std::string::npos)
		{
			str.erase(secondLastNewline + 1, lastNewline - secondLastNewline
				- 1);
		}
		else
		{
			str.erase(0, lastNewline);
		}
	}
}

void	trimFirstChar(std::string &str)
{
	str = str.substr(1, str.size());
}

void	trimLastChar(std::string &str)
{
	str = str.substr(0, str.size() - 1);
}

void	printFileStruct(file_t *file)
{
	logger.log(REQUEST, "fileName: " + file->fileName);
	logger.log(REQUEST, "fileContent: " + file->fileContent);
	logger.log(REQUEST, "fileContent.size(): "
		+ std::to_string(file->fileContent.size()));
	// for(int i = 0; i < file->fileContent.size(); i++)
	//     std::cout << file->fileContent[i] << std::endl;
	logger.log(REQUEST, "fileContentType: " + file->fileContentType);
	logger.log(REQUEST, "fileContentDisposition: "
		+ file->fileContentDisposition);
	logger.log(REQUEST, "fileBoundary: " + file->fileBoundary);
}

void removeTrailingCRLF(std::string &str)
{
    if (str.length() >= 2 && str.substr(str.length() - 2) == "\r\n")
    {
        str.erase(str.length() - 2); // Remove the last two characters if they are "\r\n"
    }
}



void findFileContent(request_t *req, file_t *requestFile)
{
    // Find the start of the content after the delimiter
    std::size_t start = req->requestBody.find("\r\n\r\n");
    if (start != std::string::npos)
    {
        start += 4;  // Move past the delimiter to the start of the content
    }
    else
    {
        logger.log(ERR, "Content start delimiter not found");
        return;
    }

    std::cout << "file boundary: " << requestFile->fileBoundary << std::endl;

    // Find the start of the boundary string
    std::size_t end = req->requestBody.find(requestFile->fileBoundary, start);
    if (end == std::string::npos)
    {
        logger.log(WARNING, "Did not find any ending boundary");
        // If boundary not found, take the rest of the string as the content
        requestFile->fileContent = req->requestBody.substr(start);
        requestFile->fileContentLength = requestFile->fileContent.size();
        return;
    }

    // The boundary might be followed by "--", adjust the end accordingly
    if (req->requestBody.compare(end - 2, 2, "\r\n") == 0)
    {
        end -= 2;  // Remove the trailing "\r\n"
    }
    
    // Now, check if there is a trailing "--" after the boundary
    if (req->requestBody.compare(end - 2, 2, "--") == 0)
    {
        end -= 2;  // Remove the trailing "--"
    }

    // Extract the file content between start and end
    requestFile->fileContent = req->requestBody.substr(start, end - start);
	removeTrailingCRLF(requestFile->fileContent);
	removeTrailingCRLF(requestFile->fileContent);
	std::cout << "|" << requestFile->fileContent << "|" << std::endl;
    requestFile->fileContentLength = requestFile->fileContent.size();

    // Uncomment this for debugging
    // std::cout << requestFile->fileContent << std::endl;
}


void	setFile(request_t *req, file_t *requestFile)
{
	std::string cLength;
	std::string totalFileContent;

	cLength = trim(extractValue(req, "Content-Length: "));
	if (req->method == POST && cLength != "0")
	{
		if (cLength == "")
			throw BadRequestException();
		req->contentLength = std::stoi(cLength);
	}
	else
	{
		req->contentLength = 0;
		logger.log(DEBUG, "Did not proceed with setFile(), contentLength = 0");
		return ;
	}
	logger.log(DEBUG, "File exists set to true in RequestFile");
	requestFile->fileExists = true;
	requestFile->fileContentType = trim(extractValue(req, "Content-Type: "));
	trimLastChar(requestFile->fileContentType);
	if (requestFile->fileContentType == "multipart/form-data")
		requestFile->fileBoundary = trim(extractValue(req,
					"Content-Type: multipart/form-data; boundary="));
	if (!requestFile->fileBoundary.empty())
	{
		try
		{
			requestFile->fileContentDisposition = trim(extractValue(req,
						"Content-Disposition: "));
			trimLastChar(requestFile->fileContentDisposition);
			requestFile->fileName = trim(extractValue(req, "filename="));
			trimFirstChar(requestFile->fileName);
			trimLastChar(requestFile->fileName);
			findFileContent(req, &req->file);
		}
		catch (const std::exception &e)
		{
			std::cout << "hier" << std::endl;
		}
	}
}