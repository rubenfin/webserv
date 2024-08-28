/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   CGI.hpp                                            :+:    :+:            */
/*                                                     +:+                    */
/*   By: rfinneru <rfinneru@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/08/24 12:43:27 by rfinneru      #+#    #+#                 */
/*   Updated: 2024/08/26 12:19:23 by rfinneru      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Webserv.hpp"

typedef struct CGI_t
{
    time_t StartTime;
    pid_t PID;
    int ReadFd;
    bool isRunning;
    CGI_t() : StartTime(0), PID(-1), ReadFd(-1), isRunning(false) {}
} CGI_t;
 
void resetCGI(CGI_t &CGI);