/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   SessionManager.hpp                                 :+:    :+:            */
/*                                                     +:+                    */
/*   By: rfinneru <rfinneru@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/12/02 15:21:35 by rfinneru      #+#    #+#                 */
/*   Updated: 2024/12/02 15:44:04 by rfinneru      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Webserv.hpp"
#include <random>

#define KEY_LENGTH 12

typedef struct SessionData
{
    std::string name;
    int value;
} SessionData_t;

class SessionManager
{
private:
    std::unordered_map<std::string, SessionData> sessions;
    
public:
    std::string createSession(const std::string &name);
    SessionData getSession(std::string sessionId);
    void deleteSession(std::string sessionId);
};

std::string SessionManager::createSession(const std::string &name)
{
    std::random_device rd;
    std::mt19937 generator(rd());
    const std::string CHARACTERS
        = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

    std::uniform_int_distribution<> distribution(0, CHARACTERS.size() - 1);

    std::string sessionId;
    for (int i = 0; i < KEY_LENGTH; ++i) {
        sessionId += CHARACTERS[distribution(generator)];
    }
    
    SessionData data{name, 1};
    sessions[sessionId] = data;
    
    return sessionId;
}

SessionData SessionManager::getSession(std::string sessionId)
{
    auto it = sessions.find(sessionId);
    if (it != sessions.end())
    {
        it->second.value++;
        return it->second;
    }
    throw std::runtime_error("Session not found");
}

void SessionManager::deleteSession(std::string sessionId)
{
    auto it = sessions.find(sessionId);
    if (it != sessions.end())
    {
        sessions.erase(it);
    }
}