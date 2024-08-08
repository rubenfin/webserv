/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Manager.cpp                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: rfinneru <rfinneru@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/08/06 13:49:20 by rfinneru      #+#    #+#                 */
/*   Updated: 2024/08/06 13:50:16 by rfinneru      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Manager.hpp"

Manager::Manager() : _epollFd(-1) {}

Manager::~Manager() {
    if (_epollFd != -1) {
        close(_epollFd);
    }
}

bool Manager::initEpoll() {
    _epollFd = epoll_create1(0);
    if (_epollFd == -1) {
        perror("epoll_create1");
        return false;
    }
    _eventList.resize(MAX_EVENTS);
    return true;
}

bool Manager::addFdToEpoll(int fd, uint32_t events) {
    struct epoll_event eventConfig;
    eventConfig.events = events;
    eventConfig.data.fd = fd;
    if (epoll_ctl(_epollFd, EPOLL_CTL_ADD, fd, &eventConfig) == -1) {
        perror("epoll_ctl ADD");
        return false;
    }
    return true;
}

bool Manager::modifyFdInEpoll(int fd, uint32_t events) {
    struct epoll_event eventConfig;
    eventConfig.events = events;
    eventConfig.data.fd = fd;
    if (epoll_ctl(_epollFd, EPOLL_CTL_MOD, fd, &eventConfig) == -1) {
        perror("epoll_ctl MOD");
        return false;
    }
    return true;
}

bool Manager::removeFdFromEpoll(int fd) {
    if (epoll_ctl(_epollFd, EPOLL_CTL_DEL, fd, nullptr) == -1) {
        perror("epoll_ctl DEL");
        return false;
    }
    return true;
}

int Manager::waitForEvents(int timeout) {
    return epoll_wait(_epollFd, _eventList.data(), MAX_EVENTS, timeout);
}

struct epoll_event* Manager::getEventList() {
    return _eventList.data();
}
