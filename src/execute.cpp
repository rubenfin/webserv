#include "../include/Webserv.hpp"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void Server::execute_CGI_script(pid_t pid, int *fds, const char *script,
	char **env)
{
	char	*exec_args[] = {(char *)script, nullptr};
	char	*envp[] = {(char *)0};

	(void)env;
	close(fds[0]);
	dup2(fds[1], STDOUT_FILENO);
	close(fds[1]);
	execve(script, exec_args, envp);
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
		bytesRead = read(fds[0], _buffer, 10000);
		std::cout << bytesRead << std::endl;
		_buffer[bytesRead] = '\0';
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
	response_t 	response;

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
		_servers[0].getHttpHandler()->handleRequest(_servers[0], &request, &response);
		std::cout << _servers[0].getHttpHandler()->getRequest()->requestURL.c_str() << std::endl;
		if (_servers[0].getHttpHandler()->getCgi())
			_servers[0].cgi(_environmentVariables);
		else
			_servers[0].readFile();
		// if (_servers[0].getHttpHandler()->getRequestBody() != "")
		// 	std::cout << "\n\n MY REQUEST BODY\n"<< _servers[0].getHttpHandler()->getRequestBody() << std::endl;
		if (send(client_socket, _servers[0].getResponse(),
				strlen(_servers[0].getResponse()), 0) == -1)
			perror("send");
		close(client_socket);
	}
	close(_servers[0].getSocketFD());
	return (0);
}
