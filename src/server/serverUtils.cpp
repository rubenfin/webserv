/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   serverUtils.cpp                                    :+:    :+:            */
/*                                                     +:+                    */
/*   By: rfinneru <rfinneru@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/24 18:40:58 by rfinneru      #+#    #+#                 */
/*   Updated: 2024/08/02 16:30:24 by rfinneru      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Webserv.hpp"

void	resetRequestResponse(request_t &request, response_t &response)
{
	logger.log(DEBUG, "Cleaning request and response struct");
	resetRequest(request);
	resetResponse(response);
}

int	checkIfDir(const std::string &pathname)
{
	struct stat	s;

	if (stat(pathname.c_str(), &s) == 0)
	{
		if (s.st_mode & S_IFDIR)
			return (1);
	}
	return (0);
}

int	checkIfFile(const std::string &pathname)
{
	struct stat s;
	if (stat(pathname.c_str(), &s) == 0)
	{
		if (s.st_mode & S_IFREG)
			return (1);
	}
	return (0);
}