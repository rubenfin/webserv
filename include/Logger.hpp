/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Logger.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: rfinneru <rfinneru@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/07/10 14:31:30 by rfinneru      #+#    #+#                 */
/*   Updated: 2024/07/10 16:26:47 by rfinneru      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Colors.hpp"
#include <string>
#include <iostream>
#include <memory>
#include <mutex>
#include <ctime> 

enum level
{
    ERR,
    WARNING,
    DEBUG,
    LOG,
    REQUEST,
    RESPONSE
};

class Logger
{
private:
    Logger();
   
   level lvl;
   std::string totalLog;
   static std::unique_ptr<Logger> instance;
   static std::once_flag initInstanceFlag;
   
public:
    static Logger& getInstance();
    void log(level lvl, const std::string &message);
    void printLog();
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    ~Logger();
};


