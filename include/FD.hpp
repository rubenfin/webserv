/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   FD.hpp                                             :+:    :+:            */
/*                                                     +:+                    */
/*   By: rfinneru <rfinneru@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/11/26 13:23:50 by rfinneru      #+#    #+#                 */
/*   Updated: 2024/11/26 14:46:35 by rfinneru      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <cstdint>
#include <limits>
#include <stdexcept>
#include <string>
#include <unistd.h>
#include <iostream>

class FileDescriptor {
private:
    int _fileFd;
    int _readFd;
    int _writeFd;
    int64_t _totalToRead;
    int64_t _bytesRead;
    int64_t _totalToWrite;
    int64_t _bytesWritten;
    bool _isOpen;

public:
    FileDescriptor();
    FileDescriptor(const FileDescriptor&);
    FileDescriptor& operator=(const FileDescriptor&) ;
    FileDescriptor(FileDescriptor&& other) noexcept ;
    FileDescriptor& operator=(FileDescriptor&& other) noexcept;
    void setFileFd(int fd);
    void setReadFd(int fd);
    void setWriteFd(int fd);
    void setTotalToRead(int64_t total);
    void setTotalToWrite(int64_t total);
    void incrementBytesRead(int64_t bytes);
    void incrementBytesWritten(int64_t bytes);
    int getFileFd() const ;
    int getReadFd() const ;
    int getWriteFd() const ;
    int64_t getTotalToRead() const ;
    int64_t getBytesRead() const;
    int64_t getTotalToWrite() const;
    int64_t getBytesWritten() const;
    bool isOpen() const;
    bool isReadComplete() const;
    bool isWriteComplete() const;
    void reset() ;
    void close();
    ~FileDescriptor();
    std::string toString() const;
};