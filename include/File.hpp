/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   File.hpp                                           :+:    :+:            */
/*                                                     +:+                    */
/*   By: rfinneru <rfinneru@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/07/09 14:31:14 by rfinneru      #+#    #+#                 */
/*   Updated: 2024/08/05 11:08:51 by rfinneru      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Request.hpp"
#include <vector>
#include <string>


typedef struct file_t
{
    bool fileChecked;
    bool fileExists;
    std::string         fileName;
    std::string         fileContent;
    std::string         fileContentType;
    std::string         fileContentDisposition;
    std::string         fileBoundary;
    ssize_t             fileContentLength;
    bool                fileEndBoundaryFound;
} file_t;

void printFileStruct(file_t *file);
void trimFirstChar(std::string &str);
void trimLastChar(std::string &str);
