/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Utils.hpp                                          :+:    :+:            */
/*                                                     +:+                    */
/*   By: rfinneru <rfinneru@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/07/31 12:43:34 by rfinneru      #+#    #+#                 */
/*   Updated: 2024/09/16 16:47:57 by rfinneru      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Webserv.hpp"

size_t	ft_strlen(const char *s);
char	*ft_strjoin(char const *s1, char const *s2);
void	resetRequestResponse(request_t &request, response_t &response);
int		checkIfDir(const std::string &pathname);
int		checkIfFile(const std::string &pathname);
void	setNonBlocking(int *fd);
bool	hasSpecialCharacters(const std::string& fileName);
void 	replaceEncodedSlash(std::string &str);
int	    configPathCheck(const std::string &filename);
void	handleSigInt(int signal);
void    initializeSignals(void);