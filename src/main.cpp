/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   main.cpp                                           :+:    :+:            */
/*                                                     +:+                    */
/*   By: jade-haa <jade-haa@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/09 14:50:29 by rfinneru      #+#    #+#                 */
/*   Updated: 2024/11/14 10:37:03 by rfinneru      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Webserv.hpp"

Logger &logger = Logger::getInstance();

int main(int argc, char **argv)
{
    if (!argumentCheck(argc, argv))
        return EXIT_FAILURE;

    try
    {
        Webserv webserv(argv[1]);
        webserv.execute();
    }
    catch(const std::exception& e)
    {
        logger.log(ERR, "Fatal error in configuration: " + std::string(e.what()));
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
