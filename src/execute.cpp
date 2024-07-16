#include "../include/Webserv.hpp"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

volatile sig_atomic_t	interrupted;

void Server::setEnv(char **&env)
{
	char	**savedEnv;
	int		existingEnvCount;

	std::vector<std::string> addedEnv;
	std::string currMethod;
	savedEnv = env;
	// Determine the current method
	if (getHttpHandler()->getRequest()->method == GET)
		currMethod = "GET";
	else if (getHttpHandler()->getRequest()->method == POST)
		currMethod = "POST";
	else if (getHttpHandler()->getRequest()->method == DELETE)
		currMethod = "DELETE";
	// Add new environment variables
	addedEnv.push_back("REQUEST_METHOD=" + currMethod);
	addedEnv.push_back("QUERY_STRING="
		+ getHttpHandler()->getRequest()->requestBody);
	auto contentTypeIt = getHttpHandler()->getRequest()->header.find("Content-Type");
	if (contentTypeIt != getHttpHandler()->getRequest()->header.end())
		addedEnv.push_back("CONTENT_TYPE=" + contentTypeIt->second);
	auto contentLengthIt = getHttpHandler()->getRequest()->header.find("Content-Length");
	if (contentLengthIt != getHttpHandler()->getRequest()->header.end())
		addedEnv.push_back("CONTENT_LENGTH=" + contentLengthIt->second);
	addedEnv.push_back("SERVER_NAME=" + getServerName());
	addedEnv.push_back("SERVER_PORT=" + std::to_string(getPort()));
	addedEnv.push_back("SCRIPT_NAME="
		+ getHttpHandler()->getRequest()->requestFile);
	addedEnv.push_back("PATH_INFO="
		+ getHttpHandler()->getRequest()->requestURL);
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

void Server::execute_CGI_script(pid_t pid, int *fds, const char *script,
	char **env)
{
	char	*exec_args[] = {(char *)script, nullptr};

	logger.log(INFO, "Executing CGI script");
	close(fds[0]);
	setEnv(env);
	// Redirect both STDOUT and STDERR
	dup2(fds[1], STDOUT_FILENO);
	dup2(fds[1], STDERR_FILENO);
	close(fds[1]);
	if (getHttpHandler()->getRequest()->method == POST)
	{
		write(STDIN_FILENO, getHttpHandler()->getRequest()->requestBody.c_str(),
			getHttpHandler()->getRequest()->requestBody.size());
		// close(STDIN_FILENO);  // Close STDIN after writing
	}
	execve(script, exec_args, env);
	// If execve returns, it failed
	perror("execve failed");
	getHttpHandler()->getResponse()->status = httpStatusCode::BadRequest;
	exit(EXIT_FAILURE);
}

void Server::cgi(char **env)
{
	pid_t	pid;
	int		fds[2];
	ssize_t	bytesRead;

	if (access(getHttpHandler()->getRequest()->requestURL.c_str(), X_OK) != 0)
	{
		logger.log(ERR, "[403] Script doesn't have executable rights");
		getHttpHandler()->getResponse()->status = httpStatusCode::Forbidden;
		throw ForbiddenException();
	}
	if (pipe(fds) == -1)
		return ;
	pid = fork();
	if (pid == 0)
		execute_CGI_script(pid, fds,
			getHttpHandler()->getRequest()->requestURL.c_str(), env);
	else
	{
		close(fds[1]);
		getHttpHandler()->getResponse()->contentLength = read(fds[0], _buffer,
				10000);
		// std::cout << getHttpHandler()->getResponse()->contentLength << std::endl;
		// logger.log(DEBUG,
			// std::to_string(getHttpHandler()->getResponse()->contentLength));
		_buffer[getHttpHandler()->getResponse()->contentLength] = '\0';
		// logger.log(INFO, "CGI SCRIPT OUTPUT\n" + (std::string)_buffer);
		close(fds[0]);
		makeResponse(_buffer);
		waitpid(pid, NULL, 0);
	}
	return ;
}

void Server::setFileInServer()
{
	int	file;

	if (_upload.empty())
	{
		logger.log(ERR,
			"[403] Tried uploading without setting an upload directory");
		getHttpHandler()->getResponse()->status = httpStatusCode::Forbidden;
		throw ForbiddenException();
	}
	else
	{
		std::string uploadPath = getUpload();
		std::string fileName = getHttpHandler()->getRequest()->file.fileName;
		std::string fileContent = getHttpHandler()->getRequest()->file.fileContent;
		std::string fullPath = uploadPath + "/" + fileName;
		if (access(fullPath.c_str(), F_OK) == 0)
			logger.log(WARNING,
				"File with same name already exists and has been overwritten");
		file = open(fullPath.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
		if (file != -1)
		{
			write(file, fileContent.c_str(), fileContent.size());
			close(file);
			getHttpHandler()->getResponse()->status = httpStatusCode::Created;
			makeResponse((char *)PAGE_201);
			logger.log(INFO, "Uploaded a file to " + uploadPath + " called "
				+ fileName);
		}
		else
		{
			logger.log(ERR, "[500] Failed to open the file: " + fullPath);
			getHttpHandler()->getResponse()->status = httpStatusCode::InternalServerError;
			throw InternalServerErrorException();
		}
	}
}

void Server::deleteFileInServer()
{
	std::string filePath = getUpload() + "/" + getHttpHandler()->getRequest()->file.fileName;

	if (getUpload().empty())
	{
		logger.log(ERR, "No upload location has been set, can't delete file");
		getHttpHandler()->getResponse()->status = httpStatusCode::BadRequest;
		throw BadRequestException();
	}
	else if (access(filePath.c_str(), F_OK) == -1)
	{
		logger.log(ERR, "Tried deleting a file that doesn't exist");
		getHttpHandler()->getResponse()->status = httpStatusCode::BadRequest;
		throw BadRequestException();
	}
	else
	{
		
		if (remove(filePath.c_str()) == 0)
		{
			logger.log(INFO, "Succesfully deleted the file located at " + filePath);
			getHttpHandler()->getResponse()->status = httpStatusCode::Accepted;
			throw AcceptedException();
		}
		else
			logger.log(ERR, "Could not delete the file located at " + filePath);
	}	
}

void	handleSigInt(int signal)
{
	if (signal == SIGINT)
	{
		logger.log(ERR, "closed Webserv with SIGINT");
		interrupted = 1;
		exit(EXIT_FAILURE);
	}
}

int Webserv::execute(void)
{
	int			client_socket;
	char		buffer[MAX_LENGTH_HTTP_REQ] = {0};
	ssize_t		valread;
	socklen_t	addrlen;
	request_t	request;
	response_t	response;

	signal(SIGINT, handleSigInt);
	// struct sockaddr_in	*address;
	addrlen = sizeof(_servers[0].getAddress());
	_servers[0].setServer();
	logger.log(INFO, "Server " + _servers[0].getServerName()
		+ " started on port " + _servers[0].getPortString());
	interrupted = 0;
	while (!interrupted)
	{
		resetRequestResponse(request, response);
		client_socket = accept(_servers[0].getSocketFD(),
				(struct sockaddr *)_servers[0].getAddress(), &addrlen);
		if (client_socket == -1)
		{
			logger.log(ERR, "[500] Error in accept()");
			_servers[0].makeResponse((char *)PAGE_500);
			if (send(client_socket, _servers[0].getResponse().c_str(),
					strlen(_servers[0].getResponse().c_str()), 0) == -1)
				logger.log(ERR, "[500] Failed to send response to client, send()");
			continue ;
		}
		valread = read(client_socket, buffer, MAX_LENGTH_HTTP_REQ - 1);
		if (valread == -1)
		{
			logger.log(ERR, "Read of client socket failed");
			_servers[0].getHttpHandler()->getResponse()->status = httpStatusCode::InternalServerError;
			_servers[0].makeResponse(getHttpStatusHTML(_servers[0].getHttpHandler()->getResponse()->status));
		}
		buffer[valread] = '\0';
		parse_request(&request, buffer);
		// logger.log(DEBUG, "Not ParsedRequest\n" + (std::string)buffer);
		try
		{
			_servers[0].getHttpHandler()->handleRequest(_servers[0], &request,
				&response);
			if (_servers[0].getHttpHandler()->getRequest()->method == DELETE)
				_servers[0].deleteFileInServer();
			else if (_servers[0].getHttpHandler()->getCgi())
				_servers[0].cgi(_environmentVariables);
			else if (_servers[0].getHttpHandler()->getRedirect())
				_servers[0].makeResponseForRedirect();
			else if (_servers[0].getHttpHandler()->getRequest()->file.fileExists)
				_servers[0].setFileInServer();
			else
				_servers[0].readFile();
		}
		catch (const HttpException &e)
		{
			if (_servers[0].getHttpHandler()->getResponse()->status == httpStatusCode::NotFound
				&& !_servers[0].getError404().empty())
			{
				_servers[0].getHttpHandler()->getRequest()->requestURL = _servers[0].getRoot()
					+ _servers[0].getError404();
				_servers[0].readFile();
			}
			else
				_servers[0].makeResponse(e.getPageContent());
		}
		logger.log(RESPONSE, _servers[0].getResponse());
		if (send(client_socket, _servers[0].getResponse().c_str(),
				strlen(_servers[0].getResponse().c_str()), 0) == -1)
			logger.log(ERR, "[500] Failed to send response to client, send()");
		close(client_socket);
	}
	close(_servers[0].getSocketFD());
	logger.log(INFO, "Server shut down");
	return (0);
}
