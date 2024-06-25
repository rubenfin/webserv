/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Exceptions.hpp                                     :+:    :+:            */
/*                                                     +:+                    */
/*   By: rfinneru <rfinneru@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/25 11:26:08 by rfinneru      #+#    #+#                 */
/*   Updated: 2024/06/25 11:39:34 by rfinneru      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#ifndef EXCEPTIONS_HPP
# define EXCEPTIONS_HPP

# include <stdexcept>

struct runTimeExceptions : public std::runtime_error
{
   runTimeExceptions() 
        : std::runtime_error("Standard runtime Exception") {}
};

#endif