/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   autoindex.cpp                                      :+:    :+:            */
/*                                                     +:+                    */
/*   By: rfinneru <rfinneru@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/07/21 11:57:13 by rfinneru      #+#    #+#                 */
/*   Updated: 2024/07/21 13:46:59 by rfinneru      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Webserv.hpp"

std::string returnAutoIndex(std::string &uri)
{
    struct dirent *de;
    std::string autoIndexFile;
    
    logger.log(DEBUG, uri);
    DIR *dr = opendir(uri.c_str());
    autoIndexFile += "<html>\n<body>";
    autoIndexFile += "Index of " + uri;
    autoIndexFile += "\n<hr>\n";
    if (dr == NULL)
    {
        logger.log(ERR, "Could not open directory in autoindex");
        throw NotFoundException();
    }

    while ((de = readdir(dr)) != NULL) 
        autoIndexFile += "<a href=\"/" + (std::string)de->d_name +  "\">" + (std::string)de->d_name + "<//a><br>\r\n";

    autoIndexFile += "\n<hr>\n</body>\n</html>";
    closedir(dr);   
    return (autoIndexFile);    
}