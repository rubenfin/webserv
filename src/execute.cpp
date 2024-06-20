#include "../include/Webserv.hpp"

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
		printf("%s\n", buffer);
		std::string content(buffer);
		_servers[0].getHttpHandler()->handleRequest(content, _servers[0]);
		if (_servers[0].getHttpHandler()->getFoundDirective()->getLocationDirective() == "/cgi-bin/")
			std::cout << "CGI NEEDS TO BE CALLED" << std::endl;
		else
			_servers[0].readFile();
		std::cout << "\n\n\n\n\n\n\n MY REQUEST BODY"<< _servers[0].getHttpHandler()->getRequestBody() << std::endl;
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
