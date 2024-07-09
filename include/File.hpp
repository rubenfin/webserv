/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   File.hpp                                           :+:    :+:            */
/*                                                     +:+                    */
/*   By: rfinneru <rfinneru@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/07/09 14:31:14 by rfinneru      #+#    #+#                 */
/*   Updated: 2024/07/09 15:09:03 by rfinneru      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Request.hpp"

typedef struct file_t
{
    std::string fileName;
    std::string fileContent;
    std::string fileContentType;
    std::string fileContentDisposition;
    std::string fileBoundary;
}   file_t;