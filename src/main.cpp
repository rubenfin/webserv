/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   main.cpp                                           :+:    :+:            */
/*                                                     +:+                    */
/*   By: jade-haa <jade-haa@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/09 14:50:29 by rfinneru      #+#    #+#                 */
/*   Updated: 2024/11/13 16:27:10 by rfinneru      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Webserv.hpp"

Logger &logger = Logger::getInstance();

bool argumentCheck(int argc, char **argv)
{
    if (argc != 2)
    {
        logger.log(ERR, "usage: ./webserv [configuration_file]");
        return false;
    }
    if (!configPathCheck(std::string(argv[1])))
    {
        logger.log(ERR, "Configuration path check failed for file: " + std::string(argv[1]));
        return false;
    }
    return true;
}

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
