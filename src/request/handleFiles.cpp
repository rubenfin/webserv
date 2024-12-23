/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   handleFiles.cpp                                    :+:    :+:            */
/*                                                     +:+                    */
/*   By: rfinneru <rfinneru@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/07/09 15:04:20 by rfinneru      #+#    #+#                 */
/*   Updated: 2024/12/12 11:56:38 by rfinneru      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Request.hpp"

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

void findFileContent(request_t &req, file_t *requestFile)
{
    std::size_t start = req.requestBody.find("\r\n\r\n");
    if (start != std::string::npos)
        start += 4; 
    else
    {
        logger.log(ERR, "Content start delimiter not found");
        return;
    }

    std::size_t end = req.requestBody.find(requestFile->fileBoundary, start);
    if (end == std::string::npos)
    {
        logger.log(WARNING, "Did not find any ending boundary");
        requestFile->fileContent = req.requestBody.substr(start);
        requestFile->fileContentLength = requestFile->fileContent.size();
        return;
    }

    if (req.requestBody.compare(end - 2, 2, "\r\n") == 0)
        end -= 2;

    if (req.requestBody.compare(end - 2, 2, "--") == 0)
        end -= 2;

    requestFile->fileContent = req.requestBody.substr(start, end - start);
    requestFile->fileContentLength = requestFile->fileContent.size();
}


void	setFile(request_t &req, file_t *requestFile)
{
	std::string cLength;
	std::string totalFileContent;

	cLength = trim(extractValue(req, "Content-Length: "));
	if (req.method == POST && cLength != "0")
	{
		if (cLength == "")
			req.badRequest = true;
		req.contentLength = std::stoi(cLength);
	}
	else
	{
		req.contentLength = 0;
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
			requestFile->fileName = extractValueDoubleQuote(req, "filename=\"");
			// trimFirstChar(requestFile->fileName);
			// trimLastChar(requestFile->fileName);
			findFileContent(req, &req.file);
		}
		catch (const std::exception &e)
		{
			std::cout << RED << e.what() << RESET << std::endl;
			throw InternalServerErrorException();
		}
	}
}