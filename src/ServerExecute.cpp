/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   ServerExecute.cpp                                  :+:    :+:            */
/*                                                     +:+                    */
/*   By: rfinneru <rfinneru@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/07/31 12:24:53 by rfinneru      #+#    #+#                 */
/*   Updated: 2024/08/09 09:59:16 by rfinneru      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"

volatile sig_atomic_t	interrupted;

void Server::setEnv(char **&env, int idx)
{
	char	**savedEnv;
	int		existingEnvCount;

	std::vector<std::string> addedEnv;
	std::string currMethod;
	savedEnv = env;
	// Determine the current method
	if (getHttpHandler(idx)->getRequest()->method == GET)
		currMethod = "GET";
	else if (getHttpHandler(idx)->getRequest()->method == POST)
		currMethod = "POST";
	else if (getHttpHandler(idx)->getRequest()->method == DELETE)
		currMethod = "DELETE";
	// Add new environment variables
	addedEnv.push_back("REQUEST_METHOD=" + currMethod);
	addedEnv.push_back("QUERY_STRING="
		+ getHttpHandler(idx)->getRequest()->requestBody);
	std::map<std::string,
		std::string>::iterator contentTypeIt = getHttpHandler(idx)->getRequest()->header.find("Content-Type");
	if (contentTypeIt != getHttpHandler(idx)->getRequest()->header.end())
		addedEnv.push_back("CONTENT_TYPE=" + contentTypeIt->second);
	std::map<std::string,
		std::string>::iterator contentLengthIt = getHttpHandler(idx)->getRequest()->header.find("Content-Length");
	if (contentLengthIt != getHttpHandler(idx)->getRequest()->header.end())
		addedEnv.push_back("CONTENT_LENGTH=" + contentLengthIt->second);
	addedEnv.push_back("SERVER_NAME=" + getServerName());
	addedEnv.push_back("SERVER_PORT=" + std::to_string(getPort()));
	addedEnv.push_back("SCRIPT_NAME="
		+ getHttpHandler(idx)->getRequest()->requestFile);
	addedEnv.push_back("PATH_INFO="
		+ getHttpHandler(idx)->getRequest()->requestURL);
	existingEnvCount = 0;
	while (savedEnv[existingEnvCount] != nullptr)
		existingEnvCount++;
	env = new char *[existingEnvCount + addedEnv.size() + 1];
	for (size_t i = 0; i < addedEnv.size(); ++i)
		env[i] = strdup(addedEnv[i].c_str());
	for (int i = 0; i < existingEnvCount; ++i)
		env[addedEnv.size() + i] = strdup(savedEnv[i]);
	env[addedEnv.size() + existingEnvCount] = nullptr;
}

void Server::execute_CGI_script(int *fds, const char *script, char **env,
	int idx)
{
	char	*exec_args[] = {(char *)script, nullptr};

	logger.log(INFO, "Executing CGI script");
	close(fds[0]);
	setEnv(env, idx);
	// Redirect both STDOUT and STDERR
	dup2(fds[1], STDOUT_FILENO);
	dup2(fds[1], STDERR_FILENO);
	close(fds[1]);
	if (getHttpHandler(idx)->getRequest()->method == POST)
	{
		write(STDIN_FILENO,
			getHttpHandler(idx)->getRequest()->requestBody.c_str(),
			getHttpHandler(idx)->getRequest()->requestBody.size());
		// close(STDIN_FILENO);  // Close STDIN after writing
	}
	execve(script, exec_args, env);
	// If execve returns, it failed
	perror("execve failed");
	getHttpHandler(idx)->getResponse()->status = httpStatusCode::BadRequest;
	exit(EXIT_FAILURE);
}

void Server::cgi(char **env, int idx)
{
	pid_t	pid;
	int		fds[2];

	logger.log(DEBUG, "in CGI");
	if (access(getHttpHandler(idx)->getRequest()->requestURL.c_str(),
			X_OK) != 0)
	{
		logger.log(ERR, "[403] Script doesn't have executable rights");
		getHttpHandler(idx)->getResponse()->status = httpStatusCode::Forbidden;
		throw ForbiddenException();
	}
	if (pipe(fds) == -1)
	{
		logger.log(ERR, "[500] Pipe has failed");
		getHttpHandler(idx)->getResponse()->status = httpStatusCode::InternalServerError;
		throw InternalServerErrorException();
	}
	pid = fork();
	if (pid == -1)
	{
		logger.log(ERR, "[500] Fork has failed");
		getHttpHandler(idx)->getResponse()->status = httpStatusCode::InternalServerError;
		throw InternalServerErrorException();
	}
	else if (pid == 0)
		execute_CGI_script(fds,
			getHttpHandler(idx)->getRequest()->requestURL.c_str(), env, idx);
	else
	{
		close(fds[1]);
		getHttpHandler(idx)->getResponse()->contentLength = read(fds[0],
				_buffer, 9999);
		_buffer[getHttpHandler(idx)->getResponse()->contentLength] = '\0';
		close(fds[0]);
		makeResponse(_buffer, idx);
		waitpid(pid, NULL, 0);
	}
	return ;
}

void Server::checkFileDetails(const int &idx, std::ofstream &file)
{
	if (_upload.empty())
	{
		logger.log(ERR,
			"[403] Tried uploading without setting an upload directory");
		getHttpHandler(idx)->getResponse()->status = httpStatusCode::Forbidden;
		throw ForbiddenException();
	}
	else
	{
		std::string uploadPath = getUpload();
		if (access(uploadPath.c_str(), F_OK) != 0)
		{
			if (mkdir(uploadPath.c_str(), 0775) == -1)
			{
				logger.log(ERR, "[500] Upload directory not found and was unable to make one");	
				throw InternalServerErrorException();
			}
			logger.log(WARNING, "Made upload dir");
		}
		std::string fileName = getHttpHandler(idx)->getRequest()->file.fileName;
		std::string fullPath = uploadPath + "/" + fileName;
		if (fileName.empty())
		{
			logger.log(ERR, "[403] No file has been uploaded");
			getHttpHandler(idx)->getResponse()->status = httpStatusCode::Forbidden;
			throw ForbiddenException();
		}
		else if (access(fullPath.c_str(), F_OK) == 0)
		{
			logger.log(WARNING,
				"File with same name already exists and has been overwritten");
			file.open(fullPath, std::ios::trunc);
			file.close();
		}
	}
	getHttpHandler(idx)->getRequest()->file.fileChecked = true;
}

void Server::setFileInServer(int idx)
{
	std::ofstream	file;
	std::string &fileContent = getHttpHandler(idx)->getRequest()->file.fileContent;
	std::string fullPath = getUpload() + "/"
		+ getHttpHandler(idx)->getRequest()->file.fileName;
	if (!getHttpHandler(idx)->getRequest()->file.fileChecked)
		checkFileDetails(idx, file);
	logger.log(DEBUG, "in setFileInServer");
	file.open(fullPath, std::ios::out | std::ios::in | std::ios::app | std::ios::binary);
	if (file.is_open())
	{
		file << fileContent;
		std::cout << getHttpHandler(idx)->getRequest()->currentBytesRead << std::endl;
		std::cout  << getHttpHandler(idx)->getRequest()->totalBytesRead  << "|" << getHttpHandler(idx)->getRequest()->contentLength << std::endl;
		
		if (getHttpHandler(idx)->getRequest()->totalBytesRead >= getHttpHandler(idx)->getRequest()->contentLength)
		{
			file.close();
			throw CreatedException();
		}
	}
	else
	{
		logger.log(ERR, "[500] Failed to create/open the file: " + fullPath);
		getHttpHandler(idx)->getResponse()->status = httpStatusCode::InternalServerError;
		throw InternalServerErrorException();
	}
}

void Server::deleteFileInServer(int idx)
{
	int	fileNameSize;

	logger.log(DEBUG, "in deleteFileInServer");
	std::string filePath = getUpload() + "/"
		+ getHttpHandler(idx)->getRequest()->file.fileName;
	fileNameSize = getHttpHandler(idx)->getRequest()->file.fileName.size();
	if (getUpload().empty())
	{
		logger.log(ERR,
			"[403] No upload location has been set,can't delete file");
		getHttpHandler(idx)->getResponse()->status = httpStatusCode::Forbidden;
		throw ForbiddenException();
	}
	else if (filePath.find("../") != std::string::npos)
	{
		logger.log(ERR,
			"[403] You can only stay in the designated upload folder");
		getHttpHandler(idx)->getResponse()->status = httpStatusCode::Forbidden;
		throw ForbiddenException();
	}
	else if (access(filePath.c_str(), F_OK) == -1)
	{
		logger.log(ERR,
			"[403] Tried deleting a file or directory that doesn't exist");
		getHttpHandler(idx)->getResponse()->status = httpStatusCode::Forbidden;
		throw ForbiddenException();
	}
	else if (checkIfDir(getUpload() + "/"
			+ getHttpHandler(idx)->getRequest()->file.fileName))
	{
		if (getHttpHandler(idx)->getRequest()->file.fileName[fileNameSize
			- 1] != '/')
		{
			logger.log(ERR,
				"[409] Tried deleting a directory with unvalid syntax  "
				+ filePath);
			getHttpHandler(idx)->getResponse()->status = httpStatusCode::Conflict;
			throw ConflictException();
		}
		else if (access(filePath.c_str(), W_OK) == -1)
		{
			logger.log(ERR, "[403] Directory does not have write permissions  "
				+ filePath);
			getHttpHandler(idx)->getResponse()->status = httpStatusCode::Forbidden;
			throw ForbiddenException();
		}
		else
		{
			if (remove(filePath.c_str()) == 0)
			{
				logger.log(INFO,
					"[204] Succesfully deleted the file located at "
					+ filePath);
				getHttpHandler(idx)->getResponse()->status = httpStatusCode::NoContent;
				throw NoContentException();
			}
			else
			{
				logger.log(ERR, "[500] Could not delete the file located at "
					+ filePath);
				getHttpHandler(idx)->getResponse()->status = httpStatusCode::InternalServerError;
				throw InternalServerErrorException();
			}
		}
	}
	if (remove(filePath.c_str()) == 0)
	{
		logger.log(INFO, "[202] Succesfully deleted the file located at "
			+ filePath);
		getHttpHandler(idx)->getResponse()->status = httpStatusCode::Accepted;
		throw AcceptedException();
	}
	else
	{
		logger.log(ERR, "[500] Could not delete the file located at "
			+ filePath);
		getHttpHandler(idx)->getResponse()->status = httpStatusCode::InternalServerError;
		throw InternalServerErrorException();
	}
}

void Server::sendResponse(const int &idx, int &socket)
{
	logger.log(INFO, "Inside sendResponse");
	logger.log(RESPONSE, getHttpHandler(idx)->getResponse()->response);
	if (send(socket, getHttpHandler(idx)->getResponse()->response.c_str(),
			strlen(getHttpHandler(idx)->getResponse()->response.c_str()), 0) ==
		-1)
	{
		logger.log(ERR, "[500] Failed to send response to client, send()");
	}
	getHttpHandler(idx)->cleanHttpHandler();
}