/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Logger.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: rfinneru <rfinneru@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/07/10 14:31:56 by rfinneru      #+#    #+#                 */
/*   Updated: 2024/07/11 08:43:29 by ruben         ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Logger.hpp"

std::unique_ptr<Logger> Logger::instance = nullptr;
std::once_flag Logger::initInstanceFlag;

std::string getCurrTime()
{
	time_t	now;
	tm		*timeinfo;
	char	timestamp[20];

	now = time(0);
	timeinfo = localtime(&now);
	strftime(timestamp, sizeof(timestamp), "%H:%M:%S", timeinfo);
    std::string formattedTime = "[" + (std::string)timestamp + "]";
    return (formattedTime);
}
Logger &Logger::getInstance()
{
	std::call_once(initInstanceFlag, []() { instance.reset(new Logger()); });
	return (*instance);
}

void Logger::log(level lvl, const std::string &message)
{
	totalLog += message;
    switch (lvl) {
        case ERR:
            std::cout << BRIGHT_RED << getCurrTime() << " [ERROR]: " << message << RESET << std::endl;
            break;
        case WARNING:
            std::cout << BRIGHT_YELLOW << getCurrTime() << " [WARNING]: " << message << RESET << std::endl;
            break;
        case DEBUG:
            std::cout << BRIGHT_GREEN << getCurrTime() << " [DEBUG]: " << message << RESET << std::endl;
            break;
        case LOG:
            std::cout << BRIGHT_WHITE << getCurrTime() << " [LOG]: " << message << RESET << std::endl;
            break;
        case REQUEST:
            std::cout << BRIGHT_BLUE << getCurrTime() << " [REQUEST]: " << message << RESET << std::endl;
            break;
         case RESPONSE:
            std::cout << BRIGHT_CYAN << getCurrTime() << " [RESPONSE]: " << message << RESET << std::endl;
            break;
        default:
            std::cerr << getCurrTime() << " [UNKNOWN]: " << message << std::endl;
            break;
    }
}

void Logger::printLog()
{
	std::cout << totalLog << std::endl;
}

Logger::Logger()
{
}

Logger::~Logger()
{
}