#include "../../include/Webserv.hpp"

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
	{
		std::cout << "The configuration is a directory" << std::endl;
		return(0);
	}
	if (!checkIfFile(filename))
	{
		std::cout << "The configuration file is not valid" << std::endl;
		return (0);
	}
    return (1);
}

void	handleSigInt(int signal)
{
	if (signal == SIGINT)
	{
		logger.log(WARNING, "closed Webserv with SIGINT");
		interrupted = 1;
	}
}

void initializeSignals(void)
{
	signal(SIGINT, handleSigInt);
	signal(SIGQUIT, SIG_IGN);
	signal(SIGPIPE, SIG_IGN);
}