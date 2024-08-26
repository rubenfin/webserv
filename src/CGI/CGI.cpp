/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   CGI.cpp                                            :+:    :+:            */
/*                                                     +:+                    */
/*   By: rfinneru <rfinneru@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/20 11:52:36 by rfinneru      #+#    #+#                 */
/*   Updated: 2024/08/26 14:02:53 by rfinneru      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/CGI.hpp"


void resetCGI(CGI_t &CGI)
{
    CGI.PID = -1;
    CGI.ReadFd = -1;
    CGI.isRunning = false;
    CGI.StartTime = 0;
}
