#include "../include/Webserv.hpp"
#define PORT 8080
#define RESPONSE "HTTP/1.1 200 OK\nContent-type: text/html\n\n<h1>Hallo</h1>\n<h2>test</h2>\n"
int execute(void)
{
    int server_fd, new_socket;
    ssize_t valread;
    struct sockaddr_in address;
    int opt = 1;
    socklen_t addrlen = sizeof(address);
    char buffer[1024] = {0};

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt,
            sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 5) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    while (true)
    {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                    &addrlen)) < 0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        valread = read(new_socket,
                        buffer,
                        1024 - 1);
        buffer[valread] = '\0';
        printf("READ BUFFER \n%s\n", buffer);
        if (send(new_socket, RESPONSE, strlen(RESPONSE), 0) == -1)
            perror("send");
        printf("SEND\n\n%s\n", RESPONSE);
        // closing the connected socket
        close(new_socket);
    }
    // closing the listening socket
    close(server_fd);
    return (0);
}