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

#include "Colors.hpp"
#include "HTTPHandler.hpp"
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <string>
#include <unistd.h>
#include <iostream>

class HTTPHandler;
class FileDescriptor;
