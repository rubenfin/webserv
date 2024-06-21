#include "../include/Webserv.hpp"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void	execute_CGI_script(pid_t pid, int *fds, const char *script, char **env)
{
	char* exec_args[] = {(char*)script, nullptr };

	(void)env;
	char *envp[] = { (char *)0 };
	close(fds[0]);
	dup2(fds[1], STDOUT_FILENO);
	close(fds[1]);
	execve(script, exec_args, envp);
	perror("execve");
	exit(EXIT_FAILURE);
}

char * Server::cgi(char **env)
{
	pid_t	pid;
	int		fds[2];
	ssize_t	bytesRead;

	if (pipe(fds) == -1)
		return (NULL);
	pid = fork();
	if (pid == 0)
		execute_CGI_script(pid, fds, getHttpHandler()->getResponseURL().c_str(), env);
	else
	{
		close(fds[1]);
		bytesRead = read(fds[0], _stringFromFile, 10000);
		std::cout << bytesRead << std::endl;
		_stringFromFile[bytesRead] = '\0';
		std::cout << "STRING FROM CGI SCRIPT\n" <<_stringFromFile << std::endl;
		close(fds[0]);
		char * header;
		// std::string message = _http_handler->getHttpStatusMessage();
		
		// header = ft_strjoin("HTTP/1.1 ", message.c_str());
		header = ft_strjoin("HTTP/1.1 200 OK", "\r\n\r\n");
		_stringFromFile = ft_strjoin(header, _stringFromFile);
		std::cout << "\n\n\n\nRESPONSE\n"<<_stringFromFile << "\n--------------------------------" << std::endl;
		waitpid(pid, NULL, 0);
	}
	return (0);
}

int Webserv::execute(void)
{
	int			client_socket;
	char		buffer[1024] = {0};
	ssize_t		valread;
	socklen_t	addrlen;

	// struct sockaddr_in	*address;
	addrlen = sizeof(_servers[0].getAddress());
	_servers[0].setServer();
	while (true)
	{
		client_socket = accept(_servers[0].getSocketFD(),
				(struct sockaddr *)_servers[0].getAddress(), &addrlen);
		if (client_socket == -1)
		{
			perror("accept");
			exit(EXIT_FAILURE);
		}
		valread = read(client_socket, buffer, 1024 - 1);
		buffer[valread] = '\0';
		// printf("%s\n", buffer);
		std::string content(buffer);
		_servers[0].getHttpHandler()->handleRequest(content, _servers[0]);
		std::cout << _servers[0].getHttpHandler()->getResponseURL().c_str() << std::endl;
		if (_servers[0].getHttpHandler()->getCgi())
			_servers[0].cgi(_environmentVariables);
		else
			_servers[0].readFile();
		std::cout << "\n\n\n\n\n\n\n MY REQUEST BODY\n"<< _servers[0].getHttpHandler()->getRequestBody() << std::endl;
		if (send(client_socket, _servers[0].getStringFromFile(),
				strlen(_servers[0].getStringFromFile()), 0) == -1)
		{
			perror("send");
		}
		// printf("SEND\n\n%s\n", _response.c_str());
		close(client_socket);
	}
	close(_servers[0].getSocketFD());
	return (0);
}
