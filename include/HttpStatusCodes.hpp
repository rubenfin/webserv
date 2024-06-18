/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   HttpStatusCodes.hpp                                :+:    :+:            */
/*                                                     +:+                    */
/*   By: rfinneru <rfinneru@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/18 14:28:52 by rfinneru      #+#    #+#                 */
/*   Updated: 2024/06/18 14:42:58 by rfinneru      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_STATUS_CODES_HPP
#define HTTP_STATUS_CODES_HPP

enum class httpStatusCode {
    OK = 200,
    Created = 201,
    Accepted = 202,
    NoContent = 204,
    MovedPermanently = 301,
    Found = 302,
    NotModified = 304,
    BadRequest = 400,
    Unauthorized = 401,
    Forbidden = 403,
    NotFound = 404,
    InternalServerError = 500,
    NotImplemented = 501,
    BadGateway = 502,
    ServiceUnavailable = 503,
    httpVersionNotSupported = 505
};

#endif