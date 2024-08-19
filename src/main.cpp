/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   main.cpp                                           :+:    :+:            */
/*                                                     +:+                    */
/*   By: jade-haa <jade-haa@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/09 14:50:29 by rfinneru      #+#    #+#                 */
/*   Updated: 2024/08/19 15:57:06 by rfinneru      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Webserv.hpp"

Logger &logger = Logger::getInstance();

int	main(int argc, char **argv, char **env)
{
	logger.setWorking(true);
	if (argc == 2)
	{
		std::string filename(argv[1]);
		if (!configPathCheck(filename))
			return (EXIT_FAILURE);
		Webserv webserv(filename, env);
		webserv.execute();
		return (EXIT_SUCCESS);
	}
	else
		return (std::cerr << "usage: ./webserv [configuration_file]" << std::endl,
			EXIT_FAILURE);
}
