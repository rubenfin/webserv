#include "../include/Webserv.hpp"
#include <fcntl.h>

size_t	ft_strlen(const char *s)
{
	int	i;

	i = 0;
	if (!s || !s[0])
		return (0);
	while (s[i] != '\0')
		++i;
	return (i);
}

char	*ft_strjoin(char const *s1, char const *s2)
{
	char	*newarray;
	int		i;
	int		j;

	newarray = (char *)malloc(ft_strlen(s1) + ft_strlen(s2) + 1);
	i = 0;
	if (newarray == NULL)
		return (NULL);
	while (s1[i] != '\0')
	{
		newarray[i] = s1[i];
		i++;
	}
	j = i;
	i = 0;
	while (s2[i] != '\0')
	{
		newarray[j] = s2[i];
		i++;
		j++;
	}
	newarray[ft_strlen(s1) + ft_strlen(s2)] = '\0';
	return (newarray);
}

char * readFile(const std::string& responseURL)
{
	char *fileInString;
	int file;
	fileInString = (char * )malloc(100001 * sizeof(char));
	std::cout << "response url " << responseURL.c_str() << std::endl;
	file = open(responseURL.c_str(), O_RDONLY);
	if (file == -1)
	{
		perror("opening file of responseURL");
		return NULL;
	}
	int rdbytes = read(file, fileInString, 100000);
	fileInString[rdbytes] = '\0';
	close(file);
	std::cout << "file In String: " << fileInString << std::endl;
	fileInString = ft_strjoin("HTTP/1.1 200 OK\n\n",fileInString);
	return(fileInString);
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
	
	std::cout << _servers[0].getRoot() << std::endl;
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
		std::string content(buffer);
		_servers[0].getHttpHandler()->handleRequest(content, _servers[0]);
		
		if (send(client_socket, readFile(_servers[0].getHttpHandler()->getResponseURL()), strlen(readFile(_servers[0].getHttpHandler()->getResponseURL())),
				0) == -1)
		{
			perror("send");
		}
		// printf("SEND\n\n%s\n", _response.c_str());
		close(client_socket);
	}
	close(_servers[0].getSocketFD());
	return (0);
}
