/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Exceptions.cpp                                     :+:    :+:            */
/*                                                     +:+                    */
/*   By: rfinneru <rfinneru@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/08/19 15:12:21 by rfinneru      #+#    #+#                 */
/*   Updated: 2024/11/12 15:42:06 by rfinneru      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Exceptions.hpp"
#include <memory>
#include <cstring>
#include <unordered_map>
#include <string>
#include <stdexcept>

std::unordered_map<std::string, std::shared_ptr<char[]>> HttpException::customPages;

HttpException::HttpException(int code, const std::string& message, char *defaultContent)
    : std::runtime_error(message), statusCode(code) {
    auto it = customPages.find(message);
    if (it != customPages.end()) {
        pageContent = it->second.get();
    } else {
        pageContent = defaultContent;
    }
}

void HttpException::setCustomPage(const std::string& exceptionName, const char* content)
{
    size_t length = std::strlen(content);
    std::shared_ptr<char[]> contentCopy(new char[length + 1]);
    std::strcpy(contentCopy.get(), content);
    customPages[exceptionName] = contentCopy;
}

char* HttpException::getPageContent() const {
    return pageContent;
} 