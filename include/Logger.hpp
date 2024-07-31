/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Logger.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: rfinneru <rfinneru@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/07/10 14:31:30 by rfinneru      #+#    #+#                 */
/*   Updated: 2024/07/31 16:31:41 by rfinneru      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Colors.hpp"
#include <string>
#include <iostream>
#include <memory>
#include <mutex>
#include <ctime> 
#include <sstream>
#include <fstream>

enum level
{
    ERR,
    WARNING,
    DEBUG,
    INFO,
    REQUEST,
    RESPONSE
};

class Logger
{
private:
    Logger();
    std::string totalLog;
    bool _working;
    static std::unique_ptr<Logger> instance;
    static std::once_flag initInstanceFlag;
   
public:
    static Logger& getInstance();
    void setWorking(bool boolean);
    void log(level lvl, const std::string &message);
    void printLog();
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    ~Logger();
};


