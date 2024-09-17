/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   ServerExecute.cpp                                  :+:    :+:            */
/*                                                     +:+                    */
/*   By: rfinneru <rfinneru@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/07/31 12:24:53 by rfinneru      #+#    #+#                 */
/*   Updated: 2024/09/17 16:33:12 by rfinneru      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"

volatile sig_atomic_t	interrupted;

char **Server::makeEnv(int idx)
{
	char	**env;

	std::vector<std::string> addingEnv;
	std::string currMethod;
	if (getHTTPHandler(idx).getRequest().method == GET)
		currMethod = "GET";
	else if (getHTTPHandler(idx).getRequest().method == POST)
		currMethod = "POST";
	else if (getHTTPHandler(idx).getRequest().method == DELETE)
		currMethod = "DELETE";
	addingEnv.push_back("REQUEST_METHOD=" + currMethod);
	addingEnv.push_back("QUERY_STRING="
		+ getHTTPHandler(idx).getRequest().requestBody);
	std::map<std::string,
		std::string>::iterator contentTypeIt = getHTTPHandler(idx).getRequest().header.find("Content-Type");
	if (contentTypeIt != getHTTPHandler(idx).getRequest().header.end())
		addingEnv.push_back("CONTENT_TYPE=" + contentTypeIt->second);
	std::map<std::string,
		std::string>::iterator contentLengthIt = getHTTPHandler(idx).getRequest().header.find("Content-Length");
	if (contentLengthIt != getHTTPHandler(idx).getRequest().header.end())
		addingEnv.push_back("CONTENT_LENGTH=" + contentLengthIt->second);
	addingEnv.push_back("SERVER_NAME=" + getServerName());
	addingEnv.push_back("SERVER_PORT=" + std::to_string(getPort()));
	addingEnv.push_back("SCRIPT_NAME="
		+ getHTTPHandler(idx).getRequest().requestFile);
	addingEnv.push_back("PATH_INFO="
		+ getHTTPHandler(idx).getRequest().requestURL);
	env = new char *[addingEnv.size() + 1];
	for (size_t i = 0; i < addingEnv.size(); ++i)
		env[i] = strdup(addingEnv[i].c_str());
	env[addingEnv.size()] = nullptr;
	return (env);
}

void Server::execute_CGI_script(int *writeSide, int *readSide,
	const char *script, int idx)
{
	char	*exec_args[] = {(char *)script, nullptr};
	char	**env;

	logger.log(INFO, "Executing CGI script");
	close(writeSide[0]);
	close(readSide[1]);
	env = makeEnv(idx);
	dup2(writeSide[1], STDOUT_FILENO);
	dup2(writeSide[1], STDERR_FILENO);
	close(writeSide[1]);
	if (getHTTPHandler(idx).getRequest().method == POST)
	{
		dup2(readSide[0], STDIN_FILENO);
	}
	execve(script, exec_args, env);
	perror("execve failed");
	getHTTPHandler(idx).getResponse().status = httpStatusCode::BadRequest;
	_exit(EXIT_FAILURE);
}

int	check_status(int status)
{
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	return (127);
}

void Server::logThrowStatus(const int &idx, const level &lvl,
	const std::string &msg, const httpStatusCode &status,
	HttpException exception)
{
	throw	exception;

	logger.log(lvl, msg);
	getHTTPHandler(idx).getResponse().status = status;
}

void Server::cgi(int idx, int socket)
{
	CGI_t	*CGIinfo;
	int		childToParent[2];
	int		parentToChild[2];
	struct epoll_event ev;

	CGIinfo = new CGI_t();
	logger.log(DEBUG, "in CGI in socket: " + std::to_string(socket));
	if (access(getHTTPHandler(idx).getRequest().requestURL.c_str(), X_OK) != 0)
		logThrowStatus(idx, ERR, "[403] Script doesn't have executable rights",
			httpStatusCode::Forbidden, ForbiddenException());
	if (pipe(childToParent) == -1)
		logThrowStatus(idx, ERR, "[500] Pipe has failed",
			httpStatusCode::InternalServerError,
			InternalServerErrorException());
	if (pipe(parentToChild) == -1)
		logThrowStatus(idx, ERR, "[500] Pipe has failed",
			httpStatusCode::InternalServerError,
			InternalServerErrorException());
	CGIinfo->PID = fork();
	if (CGIinfo->PID == -1)
		logThrowStatus(idx, ERR, "[500] Fork has failed",
			httpStatusCode::InternalServerError,
			InternalServerErrorException());
	else if (CGIinfo->PID == 0)
		execute_CGI_script(childToParent, parentToChild,
			getHTTPHandler(idx).getRequest().requestURL.c_str(), idx);
	else
	{
		close(parentToChild[0]);
		close(childToParent[1]);
		CGIinfo->ReadFd = childToParent[0];
		CGIinfo->WriteFd = parentToChild[1];
		CGIinfo->isRunning = true;
		CGIinfo->StartTime = time(NULL);
		fcntl(childToParent[0], F_SETFL, O_NONBLOCK);
		fcntl(parentToChild[1], F_SETFL, O_NONBLOCK);
		ev.events = EPOLLIN;
		ev.data.fd = childToParent[0];
		if (epoll_ctl((*_epollFDptr), EPOLL_CTL_ADD, childToParent[0], &ev) ==
			-1)
		{
			logThrowStatus(idx, ERR,
				"[500] Couldn't add childToParent FD to epoll in CGI",
				httpStatusCode::InternalServerError,
				InternalServerErrorException());
		}
		ev.events = EPOLLOUT;
		ev.data.fd = parentToChild[1];
		if (epoll_ctl((*_epollFDptr), EPOLL_CTL_ADD, parentToChild[1], &ev) ==
			-1)
		{
			logThrowStatus(idx, ERR,
				"[500] Couldn't add parentToChild FD to epoll in CGI",
				httpStatusCode::InternalServerError,
				InternalServerErrorException());
		}
		_fdsRunningCGI.insert({socket, CGIinfo});
		getHTTPHandler(idx).setConnectedToCGI(CGIinfo);
	}
	return ;
}

void Server::checkFileDetails(const int &idx, std::ofstream &file)
{
	if (_upload.empty())
		logThrowStatus(idx, ERR,
			"[403] Tried uploading without setting an upload directory",
			httpStatusCode::Forbidden, ForbiddenException());
	else
	{
		std::string uploadPath = getUpload();
		if (access(uploadPath.c_str(), F_OK) != 0)
		{
			if (mkdir(uploadPath.c_str(), 0775) == -1)
				logThrowStatus(idx, ERR,
					"[500] Upload directory not found and was unable to make one",
					httpStatusCode::InternalServerError,
					InternalServerErrorException());
			logger.log(WARNING, "Made upload dir");
		}
		std::string fileName = getHTTPHandler(idx).getRequest().file.fileName;
		std::string fullPath = uploadPath + "/" + fileName;
		if (fileName.empty())
			logThrowStatus(idx, ERR, "[403] No file has been uploaded",
				httpStatusCode::Forbidden, ForbiddenException());
		else if (access(fullPath.c_str(), F_OK) == 0)
		{
			logger.log(WARNING,
				"File with same name already exists and has been overwritten");
			file.open(fullPath, std::ios::trunc);
			file.close();
		}
	}
	getHTTPHandler(idx).getRequest().file.fileChecked = true;
}

void Server::setFileInServer(int idx)
{
	std::ofstream file;
	std::string &fileContent = getHTTPHandler(idx).getRequest().file.fileContent;
	std::string fullPath = getUpload() + "/"
		+ getHTTPHandler(idx).getRequest().file.fileName;
	if (!getHTTPHandler(idx).getRequest().file.fileChecked)
		checkFileDetails(idx, file);
	logger.log(DEBUG, "in setFileInServer");
	file.open(fullPath,
		std::ios::out | std::ios::in | std::ios::app | std::ios::binary);
	if (file.is_open())
	{
		file << fileContent;
		if (getHTTPHandler(idx).getRequest().totalBytesRead >= getHTTPHandler(idx).getRequest().contentLength)
		{
			file.close();
			logThrowStatus(idx, INFO, "[201] Done uploading file, read all the bytes of file: " + fullPath + " with size: "
				+ std::to_string(getFileSize(fullPath, idx)),
				httpStatusCode::Created, CreatedException());
		}
	}
	else
		logThrowStatus(idx, ERR, "[500] Failed to create/open the file: "
			+ fullPath, httpStatusCode::InternalServerError,
			InternalServerErrorException());
}

void Server::deleteFileInServer(int idx)
{
	int	fileNameSize;

	logger.log(DEBUG, "in deleteFileInServer");
	std::string filePath = getUpload() + "/"
		+ getHTTPHandler(idx).getRequest().file.fileName;
	fileNameSize = getHTTPHandler(idx).getRequest().file.fileName.size();
	if (getUpload().empty())
		logThrowStatus(idx, ERR, "[403] No upload location has been set, can't delete file", httpStatusCode::Forbidden,
			ForbiddenException());
	else if (filePath.find("../") != std::string::npos)
		logThrowStatus(idx, ERR,
			"[403] You can only stay in the designated upload folder",
			httpStatusCode::Forbidden, ForbiddenException());
	else if (access(filePath.c_str(), F_OK) == -1)
		logThrowStatus(idx, ERR,
			"[403] Tried deleting a file or directory that doesn't exist",
			httpStatusCode::Forbidden, ForbiddenException());
	else if (checkIfDir(getUpload() + "/"
			+ getHTTPHandler(idx).getRequest().file.fileName))
	{
		if (getHTTPHandler(idx).getRequest().file.fileName[fileNameSize
			- 1] != '/')
			logThrowStatus(idx, ERR,
				"[409] Tried deleting a directory with unvalid syntax "
				+ filePath, httpStatusCode::Conflict, ConflictException());
		else if (access(filePath.c_str(), W_OK) == -1)
			logThrowStatus(idx, ERR,
				"[403] Directory does not have write permissions " + filePath,
				httpStatusCode::Forbidden, ForbiddenException());
		else
		{
			if (remove(filePath.c_str()) == 0)
				logThrowStatus(idx, INFO,
					"[204] Succesfully deleted the file located at " + filePath,
					httpStatusCode::NoContent, NoContentException());
			else
				logThrowStatus(idx, ERR,
					"[500] Could not delete the file located at " + filePath,
					httpStatusCode::InternalServerError,
					InternalServerErrorException());
		}
	}
	if (remove(filePath.c_str()) == 0)
		logThrowStatus(idx, INFO,
			"[202] Succesfully deleted the file located at " + filePath,
			httpStatusCode::Accepted, AcceptedException());
	else
		logThrowStatus(idx, ERR, "[500] Could not delete the file located at "
			+ filePath, httpStatusCode::InternalServerError,
			InternalServerErrorException());
}

void Server::sendResponse(const int &idx, int &socket)
{
	logger.log(INFO, "Sending response to client on socket: "
		+ std::to_string(socket));
	logger.log(RESPONSE, getHTTPHandler(idx).getResponse().response);
	if (send(socket, getHTTPHandler(idx).getResponse().response.data(),
			getHTTPHandler(idx).getResponse().response.size(), 0) == -1)
	{
		logger.log(ERR, "[500] Failed to send response to client, socket is most likely closed");
	}
	getHTTPHandler(idx).cleanHTTPHandler();
	removeSocketAndServer(socket);
	removeFdFromEpoll(socket);
	close(socket);
}