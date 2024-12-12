/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   PortValidator.cpp                                  :+:    :+:            */
/*                                                     +:+                    */
/*   By: rfinneru <rfinneru@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/12/12 13:19:30 by rfinneru      #+#    #+#                 */
/*   Updated: 2024/12/12 13:37:03 by rfinneru      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/PortValidator.hpp"

void PortValidator::correctHostString(std::string &host)
{
    if (host == "localhost")
        host = "127.0.0.1";
    else if (host == "dump-linux")
        host = "127.0.1.1";
}

bool PortValidator::addToUsedPorts(std::string host, int port)
{
    correctHostString(host);
    std::pair<std::string, int> pair = std::make_pair(host, port);
    if (usedPorts.find(pair) == usedPorts.end())
    {
        usedPorts.insert(pair);
        return true;
    }
    else
       return false;
}