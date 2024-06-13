#include "../include/Webserv.hpp"

int Webserv::execute(void)
{
	int			client_socket;
	char		buffer[1024] = {0};
	ssize_t		valread;
	socklen_t	addrlen;

	// struct sockaddr_in	*address;
	_handler = new HttpHandler;
	addrlen = sizeof(_servers[0].getAddress());
	_servers[0].setServer();
	while (true)
	{
		client_socket = accept(this->_servers[0].getSocketFD(),
				(struct sockaddr *)_servers[0].getAddress(), &addrlen);
		if (client_socket == -1)
		{
			perror("accept");
			exit(EXIT_FAILURE);
		}
		valread = read(client_socket, buffer, 1024 - 1);
		buffer[valread] = '\0';
		printf("Buffer\n %s\n", buffer);
		std::string content(buffer);
		_handler->handleRequest(content);
		if (send(client_socket, _response.c_str(), strlen(_response.c_str()),
				0) == -1)
		{
			perror("send");
		}
		// printf("SEND\n\n%s\n", _response.c_str());
		close(client_socket);
	}
	close(this->_servers[0].getSocketFD());
	return (0);
}
