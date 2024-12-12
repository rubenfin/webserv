/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   PortValidator.hpp                                  :+:    :+:            */
/*                                                     +:+                    */
/*   By: rfinneru <rfinneru@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/12/12 13:02:17 by rfinneru      #+#    #+#                 */
/*   Updated: 2024/12/12 13:28:39 by rfinneru      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <set>
#include <stdexcept>
#include <stdbool.h>
#include <iostream>

class PortValidator {
private:
    std::set<std::pair<std::string, int>> usedPorts;
public:
    bool addToUsedPorts(std::string host, int port);
    void correctHostString(std::string &host);
};
