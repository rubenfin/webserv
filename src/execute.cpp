#include "../include/Webserv.hpp"
#define RESPONSE "HTTP/1.1 200 OK\nContent-type: text/html\n\n<h1>Hallo</h1>\n<h2>test</h2>\n"

int Webserv::execute(void)
{
	int new_socket;
	ssize_t valread;
	struct sockaddr_in address;
	int opt = 1;
	socklen_t addrlen = sizeof(address);
	char buffer[1024] = {0};

	this->_servers[0].setSockedFD(socket(AF_INET, SOCK_STREAM, 0));
	if (this->_servers[0].getSocketFD() < 0)
	{
		perror("socket failed");
		exit(EXIT_FAILURE);
	}
	if (setsockopt(this->_servers[0].getSocketFD(), SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt,
			sizeof(opt)))
	{
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(this->_servers[0].getPort());
	if (bind(this->_servers[0].getSocketFD(), (struct sockaddr *)&address, sizeof(address)) < 0)
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
	if (listen(this->_servers[0].getSocketFD(), 5) < 0)
	{
		perror("listen");
		exit(EXIT_FAILURE);
	}
	while (true)
	{
		if ((new_socket = accept(this->_servers[0].getSocketFD(), (struct sockaddr *)&address,
					&addrlen)) < 0)
		{
			perror("accept");
			exit(EXIT_FAILURE);
		}
		valread = read(new_socket, buffer, 1024 - 1);
		buffer[valread] = '\0';
		printf("READ BUFFER \n%s\n", buffer);
		if (send(new_socket, RESPONSE, strlen(RESPONSE), 0) == -1)
			perror("send");
		printf("SEND\n\n%s\n", RESPONSE);
		// closing the connected socket
		close(new_socket);
	}
	// closing the listening socket
	close(this->_servers[0].getSocketFD());
	return (0);
}