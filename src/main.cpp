/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   main.cpp                                           :+:    :+:            */
/*                                                     +:+                    */
/*   By: jade-haa <jade-haa@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/09 14:50:29 by rfinneru      #+#    #+#                 */
/*   Updated: 2024/11/11 18:12:10 by rfinneru      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Webserv.hpp"

Logger &logger = Logger::getInstance();

int main(int argc, char **argv)
{
    logger.setWorking(true);
    disable_ctrl_chars();

    if (argc != 2)
    {
        std::cerr << "usage: ./webserv [configuration_file]" << std::endl;
        return EXIT_FAILURE;
    }

    std::string filename(argv[1]);
    if (!configPathCheck(filename))
    {
        logger.log(ERR, "Configuration path check failed for file: " + filename);
        return EXIT_FAILURE;
    }

    try
    {
        Webserv webserv(filename);
        webserv.execute();
    }
    catch(const std::exception& e)
    {
        logger.log(ERR, "Fatal error in configuration: " + std::string(e.what()));
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
