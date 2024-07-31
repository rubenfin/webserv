#include "../../include/Webserv.hpp"

void setNonBlocking(int *sock) {
    int flags = fcntl((*sock), F_GETFL, 0);
    if (flags == -1) {
        perror("fcntl F_GETFL");
        exit(EXIT_FAILURE);
    }
    if (fcntl((*sock), F_SETFL, flags | O_NONBLOCK) == -1) {
        perror("fcntl F_SETFL O_NONBLOCK");
        exit(EXIT_FAILURE);
    }
}

bool hasSpecialCharacters(const std::string& fileName)
{
    for (char ch : fileName)
        if (!std::isalnum(ch) && ch != '.' && ch != '_' && ch != '-') 
            return true;
    return false;
}

void replaceEncodedSlash(std::string &str)
{
    const std::string encodedSlash = "%2F";
    const std::string slash = "/";
    
    size_t pos = 0;
    while ((pos = str.find(encodedSlash, pos)) != std::string::npos) {
        str.replace(pos, encodedSlash.length(), slash);
        pos += slash.length(); // Move past the replaced part
    }
}

int	configPathCheck(const std::string &filename)
{
	if (checkIfDir(filename))
		return (logger.log(ERR, "The configuration filepath is a directory"),
			0);
	if (!checkIfFile(filename))
		return (logger.log(ERR, "The configuration filepath is not correct"),
			0);
    return (1);
}