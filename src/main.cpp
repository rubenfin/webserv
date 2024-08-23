/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   main.cpp                                           :+:    :+:            */
/*                                                     +:+                    */
/*   By: jade-haa <jade-haa@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/09 14:50:29 by rfinneru      #+#    #+#                 */
/*   Updated: 2024/08/23 12:04:40 by rfinneru      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Webserv.hpp"

Logger &logger = Logger::getInstance();

int	main(int argc, char **argv)
{
	logger.setWorking(false);
	if (argc == 2)
	{
		std::string filename(argv[1]);
		if (!configPathCheck(filename))
			return (EXIT_FAILURE);
		Webserv webserv(filename);
		try
		{
			webserv.execute();
		}
		catch (const std::exception &e)
		{			
			logger.log(ERR, "Caught in try/catch webserv");
			std::cerr << e.what() << std::endl;
		}	
		return (EXIT_SUCCESS);
	}
	else
		return (std::cerr << "usage: ./webserv [configuration_file]" << std::endl,
			EXIT_FAILURE);
}
