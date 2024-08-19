/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Exceptions.cpp                                     :+:    :+:            */
/*                                                     +:+                    */
/*   By: rfinneru <rfinneru@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/08/19 15:12:21 by rfinneru      #+#    #+#                 */
/*   Updated: 2024/08/19 16:28:48 by rfinneru      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Exceptions.hpp"

// Define the static map here
std::unordered_map<std::string, char*> HttpException::customPages;

// Constructor implementation
HttpException::HttpException(int code, const std::string& message, char *defaultContent)
    : std::runtime_error(message), statusCode(code) {
    // Check if there's a custom page set for this exception type
    auto it = customPages.find(message);
    if (it != customPages.end()) {
        pageContent = it->second;  // Use custom content if available
    } else {
        pageContent = defaultContent;  // Otherwise, use default content
    }
}

void HttpException::setCustomPage(const std::string& exceptionName, const char* content)
{
    // Make a deep copy of the content to avoid issues with the original buffer
    size_t length = std::strlen(content);
    char* contentCopy = new char[length + 1];
    std::strcpy(contentCopy, content);
    customPages[exceptionName] = contentCopy;
}


// Define the getPageContent method
char* HttpException::getPageContent() const {
    return pageContent;
}

// Other method implementations (if any)
