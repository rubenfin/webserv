/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   SessionManager.hpp                                 :+:    :+:            */
/*                                                     +:+                    */
/*   By: rfinneru <rfinneru@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/12/02 15:21:35 by rfinneru      #+#    #+#                 */
/*   Updated: 2024/12/03 13:25:36 by rfinneru      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Webserv.hpp"
#include <random>

#define KEY_LENGTH 12
#define EXPIRE_TIME_S 600

struct SessionData
{
    std::string name;
    int value;
    time_t timeStarted;
};

class SessionManager
{
private:
    std::unordered_map<std::string, SessionData> sessions;
public:
    std::string createSession();
    std::shared_ptr<SessionData> getSession(std::string sessionId);
    void deleteSession(std::string sessionId);
    void checkForExpiredSessions();
};
