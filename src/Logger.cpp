/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Logger.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: jade-haa <jade-haa@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/07/10 14:31:56 by rfinneru      #+#    #+#                 */
/*   Updated: 2024/08/17 13:35:36 by rfinneru      ########   odam.nl         */
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

void Logger::setWorking(bool boolean)
{
    _working = boolean;
}

void Logger::log(level lvl, const std::string &message)
{
    if (!_working)
        return;

    std::istringstream messageStream(message);
    std::string line;

    while (std::getline(messageStream, line)) {
        totalLog += line + "\n";
        std::string timeStr = getCurrTime();
        
        switch (lvl) {
            case ERR:
                std::cout << BRIGHT_RED << timeStr << " [ERROR]: " << line << RESET << std::endl;
                break;
            case WARNING:
                std::cout << BRIGHT_YELLOW << timeStr << " [WARNING]: " << line << RESET << std::endl;
                break;
            case DEBUG:
                std::cout << BRIGHT_GREEN << timeStr << " [DEBUG]: " << line << RESET << std::endl;
                break;
            case INFO:
                std::cout << BRIGHT_WHITE << timeStr << " [INFO]: " << line << RESET << std::endl;
                break;
            case REQUEST:
                std::cout << BRIGHT_BLUE << timeStr << " [REQUEST]: " << line << RESET << std::endl;
                break;
            case RESPONSE:
                std::cout << BRIGHT_CYAN << timeStr << " [RESPONSE]: " << line << RESET << std::endl;
                break;
            default:
                std::cerr << timeStr << " [UNKNOWN]: " << line << std::endl;
                break;
        }
    }
            printLog();
}


void Logger::printLog()
{
    std::ofstream log("extra/log");
	log << totalLog;
}

Logger::Logger()
{
}

Logger::~Logger()
{
}