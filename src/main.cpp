/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   main.cpp                                           :+:    :+:            */
/*                                                     +:+                    */
/*   By: jade-haa <jade-haa@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/09 14:50:29 by rfinneru      #+#    #+#                 */
/*   Updated: 2024/07/03 16:46:39 by rfinneru      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Webserv.hpp"

int	main(int argc, char **argv, char **env)
{
	if (argc != 2)
		return (std::cerr << "usage: ./webserv [configuration_file]" << std::endl,
			EXIT_FAILURE);
	std::string filename = argv[1];
	if (checkIfDir(filename))
		return (std::cerr << "The configuration filepath is a directory" << std::endl,
			EXIT_FAILURE);
	if (!checkIfFile(filename))
		return (std::cerr << "The configuration filepath is not correct" << std::endl,
			EXIT_FAILURE);
	Webserv webserv(filename, env);
	webserv.execute();
	return (EXIT_SUCCESS);
}
