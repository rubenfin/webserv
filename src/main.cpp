/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   main.cpp                                           :+:    :+:            */
/*                                                     +:+                    */
/*   By: rfinneru <rfinneru@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/09 14:50:29 by rfinneru      #+#    #+#                 */
/*   Updated: 2024/06/11 16:42:53 by rfinneru      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Webserv.hpp"

int	main(int argc, char **argv)
{
	if (argc != 2)
		return (1);
	std::string filename = argv[1];
	Webserv webserv(filename);
	webserv.printParsing();
	webserv.execute();
	return (0);
}