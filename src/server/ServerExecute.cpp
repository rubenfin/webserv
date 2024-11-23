/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   ServerExecute.cpp                                  :+:    :+:            */
/*                                                     +:+                    */
/*   By: rfinneru <rfinneru@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/07/31 12:24:53 by rfinneru      #+#    #+#                 */
/*   Updated: 2024/11/12 16:15:38 by rfinneru      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Server.hpp"

volatile sig_atomic_t	interrupted;

char **Server::makeEnv(HTTPHandler &handler)
{
	char	**env;

	std::vector<std::string> addingEnv;
	std::string currMethod;
	if (handler.getRequest().method == GET)
		currMethod = "GET";
	else if (handler.getRequest().method == POST)
		currMethod = "POST";
	else if (handler.getRequest().method == DELETE)
		currMethod = "DELETE";
	addingEnv.push_back("REQUEST_METHOD=" + currMethod);
	addingEnv.push_back("QUERY_STRING="
		+ handler.getRequest().requestBody);
	std::map<std::string,
		std::string>::iterator contentTypeIt = handler.getRequest().header.find("Content-Type");
	if (contentTypeIt != handler.getRequest().header.end())
		addingEnv.push_back("CONTENT_TYPE=" + contentTypeIt->second);
	std::map<std::string,
		std::string>::iterator contentLengthIt = handler.getRequest().header.find("Content-Length");
	if (contentLengthIt != handler.getRequest().header.end())
		addingEnv.push_back("CONTENT_LENGTH=" + contentLengthIt->second);
	addingEnv.push_back("SERVER_NAME=" + getServerName());
	addingEnv.push_back("SERVER_PORT=" + std::to_string(getPort()));
	addingEnv.push_back("SCRIPT_NAME="
		+ handler.getRequest().requestFile);
	addingEnv.push_back("PATH_INFO="
		+ handler.getRequest().requestURL);
	env = new char *[addingEnv.size() + 1];
	for (size_t i = 0; i < addingEnv.size(); ++i)
		env[i] = strdup(addingEnv[i].c_str());
	env[addingEnv.size()] = nullptr;
	return (env);
}

void Server::execute_CGI_script(int *writeSide, int *readSide,
	const char *script, HTTPHandler &handler)
{
	char	*exec_args[] = {(char *)script, nullptr};
	char	**env;

	logger.log(INFO, "Executing CGI script");
	close(writeSide[0]);
	close(readSide[1]);
	env = makeEnv(handler);
	dup2(writeSide[1], STDOUT_FILENO);
	dup2(writeSide[1], STDERR_FILENO);
	close(writeSide[1]);
	if (handler.getRequest().method == POST)
	{
		dup2(readSide[0], STDIN_FILENO);
	}
	execve(script, exec_args, env);
	perror("execve failed");
	handler.getResponse().status = httpStatusCode::BadRequest;
	_exit(EXIT_FAILURE);
}

void Server::cgi(HTTPHandler &handler, const int& socket)
{
	CGI_t	*CGIinfo;
	int		childToParent[2];
	int		parentToChild[2];
	struct epoll_event ev;

	CGIinfo = new CGI_t();
	logger.log(DEBUG, "in CGI in socket: " + std::to_string(socket));
	if (access(handler.getRequest().requestURL.c_str(), X_OK) != 0)
	{
		delete CGIinfo;
		logThrowStatus(handler, ERR, "[403] Script doesn't have executable rights",
			httpStatusCode::Forbidden, ForbiddenException());
	}
	if (pipe(childToParent) == -1)
	{
		delete CGIinfo;
		logThrowStatus(handler, ERR, "[500] Pipe has failed",
			httpStatusCode::InternalServerError,
			InternalServerErrorException());
	}
	if (pipe(parentToChild) == -1)
	{
		delete CGIinfo;
		logThrowStatus(handler, ERR, "[500] Pipe has failed",
			httpStatusCode::InternalServerError,
			InternalServerErrorException());
	}
	CGIinfo->PID = fork();
	if (CGIinfo->PID == -1)
	{
		delete CGIinfo;
		logThrowStatus(handler, ERR, "[500] Fork has failed",
			httpStatusCode::InternalServerError,
			InternalServerErrorException());
	}
	else if (CGIinfo->PID == 0)
		execute_CGI_script(childToParent, parentToChild,
			handler.getRequest().requestURL.c_str(), handler);
	else
	{
		close(parentToChild[0]);
		close(childToParent[1]);
		CGIinfo->ReadFd = childToParent[0];
		CGIinfo->WriteFd = parentToChild[1];
		CGIinfo->isRunning = true;
		CGIinfo->StartTime = time(NULL);
		CGIinfo->LastAction = time(NULL);
		fcntl(childToParent[0], F_SETFL, O_NONBLOCK);
		fcntl(parentToChild[1], F_SETFL, O_NONBLOCK);
		ev.events = EPOLLIN;
		ev.data.fd = childToParent[0];
		if (epoll_ctl((*_epollFDptr), EPOLL_CTL_ADD, childToParent[0], &ev) ==
			-1)
		{
			delete CGIinfo;
			logThrowStatus(handler, ERR,
				"[500] Couldn't add childToParent FD to epoll in CGI",
				httpStatusCode::InternalServerError,
				InternalServerErrorException());
		}
		ev.events = EPOLLOUT;
		ev.data.fd = parentToChild[1];
		if (epoll_ctl((*_epollFDptr), EPOLL_CTL_ADD, parentToChild[1], &ev) ==
			-1)
		{
			delete CGIinfo;
			logThrowStatus(handler, ERR,
				"[500] Couldn't add parentToChild FD to epoll in CGI",
				httpStatusCode::InternalServerError,
				InternalServerErrorException());
		}
		_fdsRunningCGI.insert({socket, CGIinfo});
		handler.setConnectedToCGI(CGIinfo);
	}
	return ;
}

void Server::checkFileDetails(HTTPHandler &handler, std::ofstream &file)
{
	if (_upload.empty())
		logThrowStatus(handler, ERR,
			"[403] Tried uploading without setting an upload directory",
			httpStatusCode::Forbidden, ForbiddenException());
	else
	{
		std::string uploadPath = getUpload();
		if (access(uploadPath.c_str(), F_OK) != 0)
		{
			if (mkdir(uploadPath.c_str(), 0775) == -1)
				logThrowStatus(handler, ERR,
					"[500] Upload directory not found and was unable to make one",
					httpStatusCode::InternalServerError,
					InternalServerErrorException());
			logger.log(WARNING, "Made upload dir");
		}
		std::string fileName = handler.getRequest().file.fileName;
		if (fileName.empty() && handler.getRequest().file.fileContentType == "multipart/form-data")
			logThrowStatus(handler, ERR, "[403] No file has been uploaded",
				httpStatusCode::Forbidden, ForbiddenException());
		else
		{
			std::string tmp = uploadPath + "/tmp";
			if (access(tmp.c_str(), F_OK) != 0)
			{
				if (mkdir(tmp.c_str(), 0775) == -1)
					logThrowStatus(handler, ERR,
						"[500] tmp directory not found and unable to make one",
						httpStatusCode::InternalServerError,
						InternalServerErrorException());
			}
			handler.getRequest().file.fileName = std::tmpnam(nullptr);
			handler.getRequest().file.fileName += ".txt";
		}
		std::string fullPath = uploadPath + "/" + handler.getRequest().file.fileName;
		if (access(fullPath.c_str(), F_OK) == 0)
		{
			logger.log(WARNING,
				"File with same name already exists and has been overwritten");
			file.open(fullPath, std::ios::trunc);
			file.close();
		}
	}
	handler.getRequest().file.fileChecked = true;
}

void Server::setFileInServer(HTTPHandler &handler)
{
	std::ofstream file;
	std::string &fileContent = handler.getRequest().file.fileContent;
	if (!handler.getRequest().file.fileChecked)
		checkFileDetails(handler, file);
	std::string fullPath = getUpload() + "/"
		+ handler.getRequest().file.fileName;
	logger.log(DEBUG, "in setFileInServer");
	file.open(fullPath,
		std::ios::out | std::ios::in | std::ios::app | std::ios::binary);
	if  (handler.getRequest().file.fileContentLength != 0 && file.is_open())
	{
		file << fileContent;
		if (handler.getRequest().totalBytesRead >= handler.getRequest().contentLength)
		{
			file.close();
			logThrowStatus(handler, INFO, "[201] Done uploading file, read all the bytes of file: " + fullPath + " with size: "
				+ std::to_string(getFileSize(fullPath, handler)),
				httpStatusCode::Created, CreatedException());
		}
	}
	else if (file.is_open() && handler.getRequest().requestBody != "")
	{
		file << handler.getRequest().requestBody;
		if (handler.getRequest().currentBytesRead < BUFFERSIZE)
		{
			file.close();
			logThrowStatus(handler, INFO, "[201] Done uploading file, read all the bytes of file: " + fullPath + " with size: "
				+ std::to_string(getFileSize(fullPath, handler)),
				httpStatusCode::Created, CreatedException());
		}
	}
	else
		logThrowStatus(handler, ERR, "[500] Failed to create/open the file: "
			+ fullPath, httpStatusCode::InternalServerError,
			InternalServerErrorException());
}

void Server::deleteFileInServer(HTTPHandler &handler)
{
	int	fileNameSize;

	logger.log(DEBUG, "in deleteFileInServer");
	std::string filePath = getUpload() + "/"
		+ handler.getRequest().file.fileName;
	fileNameSize = handler.getRequest().file.fileName.size();
	if (getUpload().empty())
		logThrowStatus(handler, ERR, "[403] No upload location has been set, can't delete file", httpStatusCode::Forbidden,
			ForbiddenException());
	else if (filePath.find("../") != std::string::npos)
		logThrowStatus(handler, ERR,
			"[403] You can only stay in the designated upload folder",
			httpStatusCode::Forbidden, ForbiddenException());
	else if (access(filePath.c_str(), F_OK) == -1)
		logThrowStatus(handler, ERR,
			"[403] Tried deleting a file or directory that doesn't exist",
			httpStatusCode::Forbidden, ForbiddenException());
	else if (checkIfDir(getUpload() + "/"
			+ handler.getRequest().file.fileName))
	{
		if (handler.getRequest().file.fileName[fileNameSize
			- 1] != '/')
			logThrowStatus(handler, ERR,
				"[409] Tried deleting a directory with unvalid syntax "
				+ filePath, httpStatusCode::Conflict, ConflictException());
		else if (access(filePath.c_str(), W_OK) == -1)
			logThrowStatus(handler, ERR,
				"[403] Directory does not have write permissions " + filePath,
				httpStatusCode::Forbidden, ForbiddenException());
		else
		{
			if (remove(filePath.c_str()) == 0)
				logThrowStatus(handler, INFO,
					"[204] Succesfully deleted the file located at " + filePath,
					httpStatusCode::NoContent, NoContentException());
			else
				logThrowStatus(handler, ERR,
					"[500] Could not delete the file located at " + filePath,
					httpStatusCode::InternalServerError,
					InternalServerErrorException());
		}
	}
	if (remove(filePath.c_str()) == 0)
		logThrowStatus(handler, INFO,
			"[202] Succesfully deleted the file located at " + filePath,
			httpStatusCode::Accepted, AcceptedException());
	else
		logThrowStatus(handler, ERR, "[500] Could not delete the file located at "
			+ filePath, httpStatusCode::InternalServerError,
			InternalServerErrorException());
}

int Server::serverActions(HTTPHandler& handler, int &socket)
{
	if (handler.getReturnAutoIndex())
	{
		makeResponse((char *)returnAutoIndex(handler,
				handler.getRequest().requestURL).c_str(), handler);
	}
	else if (handler.getRequest().method == DELETE)
		deleteFileInServer(handler);
	else if (handler.getCgi())
		cgi(handler, socket);
	else if (handler.getRedirect())
		makeResponseForRedirect(handler);
	else if (handler.getRequest().file.fileExists)
		setFileInServer(handler);
	else
		readFile(handler);
	// if (handler.getRequest().currentBytesRead < BUFFERSIZE - 1
	// 	&& !handler.getChunked() && !handler.getCgi())
	// {
	// 	sendResponse(handler, socket);
	// 	return (0);
	// }
	return (1);
}
