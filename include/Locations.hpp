/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Locations.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jack <jack@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/09 16:11:43 by jade-haa          #+#    #+#             */
/*   Updated: 2024/06/09 22:35:02 by jack             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Webserv.hpp"
#include <iostream>

class Locations
{
  private:
	std::string url_;

  public:
	Locations(std::string locationContent)
	{
		this->url_ = "test";
		std::cout << locationContent << std::endl;
	}
	~Locations()
	{

	}
};
