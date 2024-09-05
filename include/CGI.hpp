/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jade-haa <jade-haa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/24 12:43:27 by rfinneru          #+#    #+#             */
/*   Updated: 2024/09/04 12:16:16 by jade-haa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Webserv.hpp"
#include <ctime>
#include <sys/types.h>

typedef struct CGI_t
{
    time_t StartTime;
    pid_t PID;
    int ReadFd;
    bool isRunning;
    CGI_t() : StartTime(0), PID(-1), ReadFd(-1), isRunning(false) {}
} CGI_t;
 
void resetCGI(CGI_t &CGI);