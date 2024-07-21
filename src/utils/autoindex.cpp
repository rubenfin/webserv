/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   autoindex.cpp                                      :+:    :+:            */
/*                                                     +:+                    */
/*   By: rfinneru <rfinneru@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/07/21 11:57:13 by rfinneru      #+#    #+#                 */
/*   Updated: 2024/07/21 15:14:59 by rfinneru      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Server.hpp"

std::string Server::returnAutoIndex(std::string &uri) {
    std::string autoIndexFile;
    std::string uriNoRoot = uri.substr(getRoot().size(), uri.size() - getRoot().size()) + "/";
    
    logger.log(DEBUG, uriNoRoot);
    
    DIR *dr = opendir(uri.c_str());
    if (dr == NULL) {
        logger.log(ERR, "Could not open directory in autoindex");
        throw NotFoundException();
    }

    autoIndexFile += "<html>\n<head><title>Index of " + uriNoRoot + "</title></head>\n<body>";
    autoIndexFile += "<h1>Index of " + uriNoRoot + "</h1>\n<hr>\n<pre>";
    autoIndexFile += "<table>\n";
    autoIndexFile += "<tr><th>Name</th><th>Last modified</th><th>Size</th></tr>\n";

    struct dirent *entry;
    struct stat statbuf;
    std::string fullPath;

    while ((entry = readdir(dr)) != NULL) {
        fullPath = uri + "/" + entry->d_name;
        
        if (stat(fullPath.c_str(), &statbuf) == -1) {
            continue;  // Skip if we can't get file info
        }

        std::string fileName = entry->d_name;
        std::string fileType = S_ISDIR(statbuf.st_mode) ? "directory" : "file";
        
        // Format last modified time
        char timeBuf[80];
        strftime(timeBuf, sizeof(timeBuf), "%Y-%m-%d %H:%M:%S", localtime(&statbuf.st_mtime));
        
        // Format file size
        std::stringstream fileSizeStream;
        if (S_ISDIR(statbuf.st_mode)) {
            fileSizeStream << "-";
        } else {
            fileSizeStream << statbuf.st_size;
        }

        autoIndexFile += "<tr>";
        autoIndexFile += "<td><a href=\"" + uriNoRoot + fileName + "\">" + fileName + "</a></td>";
        autoIndexFile += "<td>" + std::string(timeBuf) + "</td>";
        autoIndexFile += "<td align=\"right\">" + fileSizeStream.str() + "</td>";
        autoIndexFile += "</tr>\n";
    }

    autoIndexFile += "</table>\n</pre>\n<hr>\n</body>\n</html>";
    closedir(dr);

    return autoIndexFile;
}
