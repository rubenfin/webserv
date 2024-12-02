/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   FD.cpp                                             :+:    :+:            */
/*                                                     +:+                    */
/*   By: rfinneru <rfinneru@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/11/26 13:29:46 by rfinneru      #+#    #+#                 */
/*   Updated: 2024/11/26 14:50:01 by rfinneru      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "FileDescriptor.hpp"

FileDescriptor::FileDescriptor()
    : _fd(-1),
      _totalToRead(0),
      _bytesRead(0),
      _isOpen(false)
{
}

FileDescriptor::FileDescriptor(const FileDescriptor& other)
    : _fd(-1),
      _totalToRead(other._totalToRead),
      _bytesRead(other._bytesRead),
      _isOpen(other._isOpen) {}

FileDescriptor& FileDescriptor::operator=(const FileDescriptor& other) {
    if (this != &other) {
        if (_fd != -1)
            ::close(_fd);
        _fd = -1;
        _totalToRead = other._totalToRead;
        _bytesRead = other._bytesRead;
        _isOpen = other._isOpen;
    }
    return *this;
}

FileDescriptor::FileDescriptor(FileDescriptor &&other) noexcept
{
    *this = std::move(other);
}

FileDescriptor& FileDescriptor::operator=(FileDescriptor &&other) noexcept
{
    if (this != &other)
    {
        _fd = other._fd;
        _totalToRead = other._totalToRead;
        _bytesRead = other._bytesRead;
        _isOpen = other._isOpen;
        other._isOpen = false;
    }
    return *this;
}

void FileDescriptor::setFileFd(int fd)
{
    _fd = fd;
    _isOpen = true;
}


void FileDescriptor::setTotalToRead(int64_t total)
{
    _totalToRead = total;
}

void FileDescriptor::incrementBytesRead(int64_t bytes)
{
    if (bytes < 0 || (_bytesRead + bytes > _totalToRead && _totalToRead > 0))
    {
        std::cerr << RED << "Incremented bytes read is out of bounds!" << RESET << std::endl;
    }
    _bytesRead += bytes;
}

int FileDescriptor::getFileFd() const
{
    return _fd;
}

int64_t FileDescriptor::getTotalToRead() const
{
    return _totalToRead;
}

int64_t FileDescriptor::getBytesRead() const
{
    return _bytesRead;
}

bool FileDescriptor::isOpen() const
{
    return _isOpen;
}

bool FileDescriptor::isReadComplete() const
{
    return _bytesRead >= _totalToRead;
}

void FileDescriptor::setHandler(HTTPHandler* handler)
{
    _connectedToHandler = handler;
}

HTTPHandler* FileDescriptor::getHandler(void)
{
    return (_connectedToHandler);
}

void FileDescriptor::clean()
{
    if (_fd != -1)
        close(_fd);
    _fd = -1;
    _totalToRead = 0;
    _bytesRead = 0;
    _isOpen = false;
}

FileDescriptor::~FileDescriptor()
{
    clean();
}