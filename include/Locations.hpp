/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Locations.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jade-haa <jade-haa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/09 16:11:43 by jade-haa          #+#    #+#             */
/*   Updated: 2024/06/09 16:17:46 by jade-haa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Webserv.hpp"
#include "Server.hpp"

class Locations : public Server
{
private:
     std::string url_;
     
public:
    Locations(/* args */);
    ~Locations();
};

Locations::Locations(/* args */)
{
}

Locations::~Locations()
{
}
