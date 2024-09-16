/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   CGI.hpp                                            :+:    :+:            */
/*                                                     +:+                    */
/*   By: rfinneru <rfinneru@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/08/24 12:43:27 by rfinneru      #+#    #+#                 */
/*   Updated: 2024/09/16 16:51:27 by rfinneru      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Webserv.hpp"
#include <time.h>
#include <unistd.h>

typedef struct CGI_t
{
    time_t StartTime;
    pid_t PID;
    int ReadFd;
    int WriteFd;
    bool isRunning;
    CGI_t() : StartTime(0), PID(-1), ReadFd(-1), WriteFd(-1), isRunning(false) {}
} CGI_t;
 
void resetCGI(CGI_t &CGI);