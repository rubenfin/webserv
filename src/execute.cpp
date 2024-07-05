#include "../include/Webserv.hpp"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

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

void Server::execute_CGI_script(pid_t pid, int *fds, const char *script, char **env) {
    char *exec_args[] = {(char *)script, nullptr};

    close(fds[0]);
    setEnv(env);

    // Redirect both STDOUT and STDERR
    dup2(fds[1], STDOUT_FILENO);
    dup2(fds[1], STDERR_FILENO);
    close(fds[1]);

    if (getHttpHandler()->getRequest()->method == POST) {
        write(STDIN_FILENO, getHttpHandler()->getRequest()->requestBody.c_str(), getHttpHandler()->getRequest()->requestBody.size());
        // close(STDIN_FILENO);  // Close STDIN after writing
    }

    execve(script, exec_args, env);

    // If execve returns, it failed
    perror("execve failed");
    getHttpHandler()->getResponse()->status = httpStatusCode::BadRequest;
    exit(EXIT_FAILURE);
}

char *Server::cgi(char **env)
{
	pid_t	pid;
	int		fds[2];
	ssize_t	bytesRead;

	if (pipe(fds) == -1)
		return (NULL);
	pid = fork();
	if (pid == 0)
		execute_CGI_script(pid, fds,
			getHttpHandler()->getRequest()->requestURL.c_str(), env);
	else
	{
		close(fds[1]);
		getHttpHandler()->getResponse()->contentLength = read(fds[0], _buffer,
				10000);
		std::cout << getHttpHandler()->getResponse()->contentLength << std::endl;
		_buffer[getHttpHandler()->getResponse()->contentLength] = '\0';
		std::cout << "STRING FROM CGI SCRIPT\n" << _buffer << std::endl;
		close(fds[0]);
		makeResponse(_buffer);
		waitpid(pid, NULL, 0);
	}
	return (0);
}

int Webserv::execute(void)
{
	int			client_socket;
	char		buffer[MAX_LENGTH_HTTP_REQ] = {0};
	ssize_t		valread;
	socklen_t	addrlen;
	request_t	request;
	response_t	response;

	// struct sockaddr_in	*address;
	addrlen = sizeof(_servers[0].getAddress());
	_servers[0].setServer();
	while (true)
	{
		resetRequestResponse(request, response);
		client_socket = accept(_servers[0].getSocketFD(),
				(struct sockaddr *)_servers[0].getAddress(), &addrlen);
		if (client_socket == -1)
		{
			perror("accept");
			exit(EXIT_FAILURE);
		}
		valread = read(client_socket, buffer, 1024 - 1);
		buffer[valread] = '\0';
		parse_request(&request, buffer);
		// printf("%s\n", buffer);
		_servers[0].getHttpHandler()->handleRequest(_servers[0], &request,
			&response);
		// std::cout << _servers[0].getHttpHandler()->getRequest()->requestURL.c_str() << std::endl;
		if (_servers[0].getHttpHandler()->getCgi())
			_servers[0].cgi(_environmentVariables);
		else if(_servers[0].getHttpHandler()->getRedirect())
			_servers[0].makeResponse(NULL);
		else 
			_servers[0].readFile();
		// if (_servers[0].getHttpHandler()->getRequestBody() != "")
		// 	std::cout << "\n\n MY REQUEST BODY\n"<< _servers[0].getHttpHandler()->getRequestBody() << std::endl;
		std::cout << "RESPONSE\n" << _servers[0].getResponse().c_str() << std::endl;
		if (send(client_socket, _servers[0].getResponse().c_str(),
				strlen(_servers[0].getResponse().c_str()), 0) == -1)
			perror("send");
		close(client_socket);
	}
	close(_servers[0].getSocketFD());
	return (0);
}
