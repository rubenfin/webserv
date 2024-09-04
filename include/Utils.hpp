/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jade-haa <jade-haa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/31 12:43:34 by rfinneru          #+#    #+#             */
/*   Updated: 2024/09/03 16:44:58 by jade-haa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Webserv.hpp"
#include <stdio.h>
#include <string.h>
#define BUFFERSIZE 1024

size_t	ft_strlen(const char *s);
char	*ft_strjoin(char const *s1, char const *s2);
void	resetRequestResponse(request_t &request, response_t &response);
int		checkIfDir(const std::string &pathname);
int		checkIfFile(const std::string &pathname);
char	*ft_read(size_t fd, char buffer[BUFFERSIZE], int *bytes_read);
void	setNonBlocking(int *fd);
bool	hasSpecialCharacters(const std::string &fileName);
void	replaceEncodedSlash(std::string &str);
int		configPathCheck(const std::string &filename);