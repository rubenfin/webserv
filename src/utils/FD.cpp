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

#include "FD.hpp"

FileDescriptor::FileDescriptor()
    : _fileFd(-1),
      _readFd(-1),
      _writeFd(-1),
      _totalToRead(0),
      _bytesRead(0),
      _totalToWrite(0),
      _bytesWritten(0),
      _isOpen(false)
{
}

    FileDescriptor::FileDescriptor(const FileDescriptor& other)
        : _fileFd(-1), _readFd(-1), _writeFd(-1),  // Initialize new descriptors as closed
          _totalToRead(other._totalToRead),
          _bytesRead(other._bytesRead),
          _totalToWrite(other._totalToWrite),
          _bytesWritten(other._bytesWritten),
          _isOpen(other._isOpen) {}

    // Copy assignment operator
    FileDescriptor& FileDescriptor::operator=(const FileDescriptor& other) {
        if (this != &other) {
            // Close current file descriptors if they are open
            if (_fileFd != -1) ::close(_fileFd);
            if (_readFd != -1) ::close(_readFd);
            if (_writeFd != -1) ::close(_writeFd);

            // Copy state
            _fileFd = -1;  // File descriptors are not copied; initialize as closed
            _readFd = -1;
            _writeFd = -1;
            _totalToRead = other._totalToRead;
            _bytesRead = other._bytesRead;
            _totalToWrite = other._totalToWrite;
            _bytesWritten = other._bytesWritten;
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
        close();
        _fileFd = other._readFd;
        _readFd = other._readFd;
        _writeFd = other._writeFd;
        _totalToRead = other._totalToRead;
        _bytesRead = other._bytesRead;
        _totalToWrite = other._totalToWrite;
        _bytesWritten = other._bytesWritten;
        _isOpen = other._isOpen;

        other._fileFd = -1;
        other._readFd = -1;
        other._writeFd = -1;
        other._isOpen = false;
    }
    return *this;
}

void FileDescriptor::setFileFd(int fd)
{
    _fileFd = fd;
    _isOpen = true;
}

void FileDescriptor::setReadFd(int fd)
{
    _readFd = fd;
    _isOpen = true;
}

void FileDescriptor::setWriteFd(int fd)
{
    _writeFd = fd;
    _isOpen = true;
}

void FileDescriptor::setTotalToRead(int64_t total)
{
    _totalToRead = total;
}

void FileDescriptor::setTotalToWrite(int64_t total)
{
    _totalToWrite = total;
}

void FileDescriptor::incrementBytesRead(int64_t bytes)
{
    if (bytes < 0 || (_bytesRead + bytes > _totalToRead && _totalToRead > 0))
    {
        std::cout << "bytes: " << bytes << "byteswritten+bytes: " << _bytesWritten + bytes << "totaltowrite: " << _totalToWrite << std::endl;
    }
    _bytesRead += bytes;
}

void FileDescriptor::incrementBytesWritten(int64_t bytes)
{
    if (bytes < 0 || (_bytesWritten + bytes > _totalToWrite && _totalToWrite > 0))
    {
        std::cout << "bytes: " << bytes << "byteswritten+bytes: " << _bytesWritten + bytes << "totaltowrite: " << _totalToWrite << std::endl;
    }
    _bytesWritten += bytes;
}

int FileDescriptor::getFileFd() const
{
    return _fileFd;
}

int FileDescriptor::getReadFd() const
{
    return _readFd;
}

int FileDescriptor::getWriteFd() const
{
    return _writeFd;
}

int64_t FileDescriptor::getTotalToRead() const
{
    return _totalToRead;
}

int64_t FileDescriptor::getBytesRead() const
{
    return _bytesRead;
}

int64_t FileDescriptor::getTotalToWrite() const
{
    return _totalToWrite;
}

int64_t FileDescriptor::getBytesWritten() const
{
    return _bytesWritten;
}

bool FileDescriptor::isOpen() const
{
    return _isOpen;
}

bool FileDescriptor::isReadComplete() const
{
    return _bytesRead >= _totalToRead && _totalToRead > 0;
}

bool FileDescriptor::isWriteComplete() const
{
    return _bytesWritten >= _totalToWrite && _totalToWrite > 0;
}

void FileDescriptor::reset()
{
    close();
    _totalToRead = 0;
    _bytesRead = 0;
    _totalToWrite = 0;
    _bytesWritten = 0;
}

void FileDescriptor::close()
{
     if (_fileFd != -1)
    {
        ::close(_fileFd);
        _fileFd = -1;
    }
    if (_readFd != -1)
    {
        ::close(_readFd);
        _readFd = -1;
    }
    if (_writeFd != -1)
    {
        ::close(_writeFd);
        _writeFd = -1;
    }
    _isOpen = false;
}

FileDescriptor::~FileDescriptor()
{
    close();
}

std::string FileDescriptor::toString() const
{
    return "FD{fileFd=" + std::to_string(_fileFd) +
            ", read=" + std::to_string(_readFd) +
           ", write=" + std::to_string(_writeFd) +
           ", totalRead=" + std::to_string(_totalToRead) +
           ", bytesRead=" + std::to_string(_bytesRead) +
           ", totalWrite=" + std::to_string(_totalToWrite) +
           ", bytesWritten=" + std::to_string(_bytesWritten) + "}";
}