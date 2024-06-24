/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   serverUtils.cpp                                    :+:    :+:            */
/*                                                     +:+                    */
/*   By: rfinneru <rfinneru@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/24 18:40:58 by rfinneru      #+#    #+#                 */
/*   Updated: 2024/06/24 18:44:08 by rfinneru      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Webserv.hpp"

void resetRequestResponse(request_t &request, response_t &response)
{
    resetRequest(request);
    resetResponse(response);
}
