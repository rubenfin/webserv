/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Manager.hpp                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: rfinneru <rfinneru@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/08/06 13:45:39 by rfinneru      #+#    #+#                 */
/*   Updated: 2024/08/06 13:54:27 by rfinneru      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <sys/epoll.h>
#include <vector>
#include <iostream>
#include <unistd.h>

#define MAX_EVENTS 16

class Manager {
public:
    Manager();
    ~Manager();

    bool initEpoll();
    bool addFdToEpoll(int fd, uint32_t events);
    bool modifyFdInEpoll(int fd, uint32_t events);
    bool removeFdFromEpoll(int fd);
    int waitForEvents(int timeout);
    struct epoll_event* getEventList();

private:
    int _epollFd;
    std::vector<struct epoll_event> _eventList;
};