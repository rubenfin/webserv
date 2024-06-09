/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jade-haa <jade-haa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/09 15:40:25 by jade-haa          #+#    #+#             */
/*   Updated: 2024/06/09 17:10:01 by jade-haa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <unordered_set>
#include <string>


enum class HttpMethod {
    GET,
    POST,
    PUT,
    DELETE,
    PATCH
};

class Server
{
  protected:
	std::string serverName_;
	uint16_t port_;
    std::string root_;
    std::string index_;
    std::unordered_set<HttpMethod> allowMethods_;
    
  public:
};