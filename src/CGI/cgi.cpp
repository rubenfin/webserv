/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   cgi.cpp                                            :+:    :+:            */
/*                                                     +:+                    */
/*   By: rfinneru <rfinneru@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/20 11:52:36 by rfinneru      #+#    #+#                 */
/*   Updated: 2024/06/20 13:44:28 by rfinneru      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Webserv.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void execute_CGI_script(pid_t pid, int *fds, char **av, char **envp)
{
    close(fds[0]);
    
    dup2(fds[1], STDOUT_FILENO);
    close(fds[1]);

    char *exec_args[] = {av[1], NULL};

    execve(av[1], exec_args, envp);
    
    perror("execve");
    exit(EXIT_FAILURE);
}

int main(int ac, char **av, char **envp)
{
    pid_t pid;
    int fds[2];
    
    if (ac != 2)
        return(printf("not 2 arguments\n"), EXIT_FAILURE);

    if (pipe(fds) == -1)
        return(write(STDERR_FILENO, "Pipe failed\n", 12), EXIT_FAILURE);
    
    pid = fork();
    if (pid == 0)
        execute_CGI_script(pid, fds, av, envp);
    else
    {
        close (fds[1]);
         char buffer[1024];
        ssize_t bytesRead;
        while ((bytesRead = read(fds[0], buffer, sizeof(buffer) - 1)) > 0) {
            buffer[bytesRead] = '\0';
            printf("%s", buffer);
        }
        
        close(fds[0]);
        
        waitpid(pid , NULL, 0);
    }
    return(0);
}
