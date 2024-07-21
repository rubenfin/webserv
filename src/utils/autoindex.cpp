/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   autoindex.cpp                                      :+:    :+:            */
/*                                                     +:+                    */
/*   By: rfinneru <rfinneru@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/07/21 11:57:13 by rfinneru      #+#    #+#                 */
/*   Updated: 2024/07/21 14:57:04 by rfinneru      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Server.hpp"

std::string Server::returnAutoIndex(std::string &uri)
{
    struct dirent *de;
    std::string autoIndexFile;
    std::string uriNoRoot;
    
    uriNoRoot = uri.substr(getRoot().size(), uri.size() - getRoot().size()) + "/";
    logger.log(DEBUG, uriNoRoot);
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
        autoIndexFile += "<a href=\"" + uriNoRoot + (std::string)de->d_name +  "\">" + (std::string)de->d_name + "<//a><br>\r\n";

    autoIndexFile += "\n<hr>\n</body>\n</html>";
    closedir(dr);   
    return (autoIndexFile);    
}
