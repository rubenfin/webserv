/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   autoindex.cpp                                      :+:    :+:            */
/*                                                     +:+                    */
/*   By: rfinneru <rfinneru@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/07/21 11:57:13 by rfinneru      #+#    #+#                 */
/*   Updated: 2024/09/18 12:02:57 by rfinneru      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Server.hpp"
std::string Server::returnAutoIndex(HTTPHandler &handler, std::string &uri) {
    std::string autoIndexFile;

    std::string root = getRoot();
    if (uri.size() < root.size())
        logThrowStatus(handler, ERR , "[404] URI is shorter than root path in autoindex" + uri + "|" + root, httpStatusCode::NotFound ,NotFoundException());
    
    std::string uriNoRoot = uri.substr(root.size());
    if (uriNoRoot.empty() || uriNoRoot.back() != '/') {
        uriNoRoot += '/';
    }
    
    logger.log(DEBUG, "in returnAutoIndex");
    logger.log(DEBUG, "uri: " + uri);
    logger.log(DEBUG, "uriNoRoot: " + uriNoRoot);

    DIR *dr = opendir(uri.c_str());
    if (dr == NULL) 
        logThrowStatus(handler, ERR , "[404] Could not open directory in autoindex", httpStatusCode::NotFound ,NotFoundException());

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
            logger.log(WARNING, "Could not get file info for: " + fullPath);
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
        autoIndexFile += "<td><a href=\"" + uriNoRoot + fileName + "\">" + fileName;
        if (checkIfDir(fullPath))
            autoIndexFile += "/";
        autoIndexFile +="</a></td>";
        autoIndexFile += "<td>" + std::string(timeBuf) + "</td>";
        autoIndexFile += "<td align=\"right\">" + fileSizeStream.str() + "</td>";
        autoIndexFile += "</tr>\n";
    }

    autoIndexFile += "</table>\n</pre>\n<hr>\n</body>\n</html>";
    closedir(dr);

    return autoIndexFile;
}

