/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   main.cpp                                           :+:    :+:            */
/*                                                     +:+                    */
/*   By: jade-haa <jade-haa@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/09 14:50:29 by rfinneru      #+#    #+#                 */
/*   Updated: 2024/07/13 21:27:59 by ruben         ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Webserv.hpp"

Logger &logger = Logger::getInstance();

int main(int argc, char **argv, char **env)
{
	if (argc != 2)
		return (std::cerr << "usage: ./webserv [configuration_file]" << std::endl,
				EXIT_FAILURE);
	std::string filename = argv[1];
	if (checkIfDir(filename))
		return (logger.log(ERR, "The configuration filepath is a directory"),
				EXIT_FAILURE);
	if (!checkIfFile(filename))
		return (logger.log(ERR, "The configuration filepath is not correct"),
				EXIT_FAILURE);
	Webserv webserv(filename, env);
	webserv.execute();
	return (EXIT_SUCCESS);
}
