/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   SessionManager.cpp                                 :+:    :+:            */
/*                                                     +:+                    */
/*   By: rfinneru <rfinneru@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/12/02 17:34:54 by rfinneru      #+#    #+#                 */
/*   Updated: 2024/12/03 15:04:38 by rfinneru      ########   odam.nl         */
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

std::string SessionManager::createSession()
{
    std::string sessionId;
    do 
    {
        sessionId = generateKey();
    } while (sessions.find(sessionId) != sessions.end());

    logger.log(INFO, "Created a new session with: " + sessionId);

    time_t currTime = time(NULL);
    sessions[sessionId] = {sessionId, 0, currTime};
    return sessionId;
}

std::shared_ptr<SessionData> SessionManager::getSession(std::string sessionId)
{
    checkForExpiredSessions();
    
    auto it = sessions.find(sessionId);
    if (it != sessions.end())
    {
        it->second.value++;
        logger.log(INFO, "Found active session with sessionId: " + sessionId);
        logger.log(INFO, "Total amount of times this session has been retrieved: " + std::to_string(it->second.value));
        return std::make_shared<SessionData>(it->second);
    }
    logger.log(INFO, "Didn't find any active session with sessionId: " + sessionId);
    return (nullptr);
}

void SessionManager::deleteSession(std::string sessionId)
{
    auto it = sessions.find(sessionId);
    if (it != sessions.end())
    {
        sessions.erase(it);
    }
}

void SessionManager::checkForExpiredSessions()
{
    time_t currentTime = time(NULL);

    for (auto it = sessions.begin(); it != sessions.end();)
    {
        if (currentTime > it->second.timeStarted + 600) 
        {
            logger.log(WARNING, "Removed an expired session from Sessions with sessionId: " + it->first);
            it = sessions.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

