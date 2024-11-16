/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Utils.hpp                                          :+:    :+:            */
/*                                                     +:+                    */
/*   By: rfinneru <rfinneru@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/07/31 12:43:34 by rfinneru      #+#    #+#                 */
/*   Updated: 2024/11/14 10:37:24 by rfinneru      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Webserv.hpp"
#include<termios.h>

size_t	ft_strlen(const char *s);
char	*ft_strjoin(char const *s1, char const *s2);
void	resetRequestResponse(request_t &request, response_t &response);
int		checkIfDir(const std::string &pathname);
int		checkIfFile(const std::string &pathname);
bool	hasSpecialCharacters(const std::string& fileName);
void 	replaceEncodedSlash(std::string &str);
int	    configPathCheck(const std::string &filename);
void	handleSigInt(int signal);
void    initializeSignals(void);
void    disable_ctrl_chars();
int	    makeSocketNonBlocking(int &sfd);
void	removeBoundaryLine(std::string &str, const std::string &boundary);
int	    fd_is_valid(int fd);
void    resetCGI(CGI_t &CGI);
int	    check_status(int status);
void    throwException(HttpException &exception);
void	my_epoll_add(int epoll_fd, int fd, uint32_t events);
bool    argumentCheck(int argc, char **argv);