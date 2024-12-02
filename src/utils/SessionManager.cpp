/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   SessionManager.cpp                                 :+:    :+:            */
/*                                                     +:+                    */
/*   By: rfinneru <rfinneru@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/12/02 17:34:54 by rfinneru      #+#    #+#                 */
/*   Updated: 2024/12/02 20:04:26 by rfinneru      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/SessionManager.hpp"

std::string generateKey()
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
    return sessionId;
}

std::string SessionManager::createSession(const std::string &name)
{
    std::string sessionId;
    do 
    {
        sessionId = generateKey();
    } while (sessions.find(sessionId) != sessions.end());

    sessions[sessionId] = {name, 0};
    return sessionId;
}

SessionData SessionManager::getSession(std::string sessionId)
{

    // std::cout << "|" << sessionId << "|" << std::endl;
    trim(sessionId);
    for (auto &e : sessions)
    {   
        std::cout << "s: |";
        std::cout << e.first << "|" << std::endl;        
    }

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