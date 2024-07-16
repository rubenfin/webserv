/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   File.hpp                                           :+:    :+:            */
/*                                                     +:+                    */
/*   By: rfinneru <rfinneru@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/07/09 14:31:14 by rfinneru      #+#    #+#                 */
/*   Updated: 2024/07/16 13:08:34 by rfinneru      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Request.hpp"

typedef struct file_t
{
    bool fileExists;
    std::string fileName;
    std::string fileContent;
    std::string fileContentType;
    std::string fileContentDisposition;
    std::string fileBoundary;
} file_t;

void printFileStruct(file_t *file);
void trimFirstChar(std::string &str);
void trimLastChar(std::string &str);
