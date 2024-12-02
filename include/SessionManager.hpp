/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   SessionManager.hpp                                 :+:    :+:            */
/*                                                     +:+                    */
/*   By: rfinneru <rfinneru@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/12/02 15:21:35 by rfinneru      #+#    #+#                 */
/*   Updated: 2024/12/02 17:46:18 by rfinneru      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Webserv.hpp"
#include <random>

#define KEY_LENGTH 12

struct SessionData
{
    std::string name;
    int value;
};

class SessionManager
{
private:
    std::unordered_map<std::string, SessionData> sessions;
public:
    std::string createSession(const std::string &name);
    SessionData getSession(std::string sessionId);
    void deleteSession(std::string sessionId);
};
