/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jade-haa <jade-haa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/09 14:50:29 by rfinneru          #+#    #+#             */
/*   Updated: 2024/06/13 16:16:30 by jade-haa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Webserv.hpp"

int	main(int argc, char **argv)
{
	if (argc != 2)
		return (1);
	std::string filename = argv[1];
	Webserv webserv(filename);
	// webserv.printParsing();
	webserv.execute();
	return (0);
}
