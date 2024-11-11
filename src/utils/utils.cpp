#include "../../include/Webserv.hpp"

bool hasSpecialCharacters(const std::string& fileName)
{
    for (char ch : fileName)
        if (!std::isalnum(ch) && ch != '.' && ch != '_' && ch != '-' && ch != ' ') 
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
        pos += slash.length();
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
		logger.log(DEBUG, "Closing Webserv, can take a few seconds...");
		interrupted = 1;
	}
}

void initializeSignals(void)
{
	signal(SIGINT, handleSigInt);
	signal(SIGQUIT, SIG_IGN);
	signal(SIGPIPE, SIG_IGN);
}

void disable_ctrl_chars() {
    struct termios tty;

    if (tcgetattr(STDIN_FILENO, &tty) != 0) {
        perror("tcgetattr");
        exit(EXIT_FAILURE);
    }

    tty.c_lflag &= ~(ICANON | ECHO);
    tty.c_iflag &= ~(IXON | IXOFF);

    if (tcsetattr(STDIN_FILENO, TCSANOW, &tty) != 0) {
        perror("tcsetattr");
        exit(EXIT_FAILURE);
    }
}

int	makeSocketNonBlocking(int &sfd)
{
	int	flags;

	flags = fcntl(sfd, F_SETFL, O_NONBLOCK, 0);
	if (flags == -1)
	{
		perror("fcntl first if");
		return (0);
	}
	return (1);
}

void	removeBoundaryLine(std::string &str, const std::string &boundary)
{
	size_t	lineStart;
	size_t	lineEnd;
	size_t	found;

	std::string boundaryLine = boundary + "--";
	found = str.find(boundaryLine);
	if (found != std::string::npos)
	{
		lineStart = str.rfind('\n', found);
		lineStart == std::string::npos ? lineStart = 0 : lineStart += 1;
		lineEnd = str.find('\n', found);
		if (lineEnd == std::string::npos)
		{
			lineEnd = str.length();
		}
		str.erase(lineStart, lineEnd - lineStart + 1);
	}
}

int	fd_is_valid(int fd)
{
	errno = 0;
	return (fcntl(fd, F_GETFD) != -1 || errno != EBADF);
}

void resetCGI(CGI_t &CGI)
{
    CGI.PID = -1;
    CGI.ReadFd = -1;
    CGI.WriteFd = -1;
    CGI.isRunning = false;
    CGI.StartTime = 0;
	CGI.LastAction = 0;
}

int	check_status(int status)
{
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	return (127);
}

void throwException(HttpException &exception)
{
	throw	exception;
}

void	my_epoll_add(int epoll_fd, int fd, uint32_t events)
{
	struct epoll_event	event;

	memset(&event, 0, sizeof(struct epoll_event));
	event.events = events;
	event.data.fd = fd;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event) < 0)
	{
		printf("epoll_ctl\n");
		perror("epoll add");
	}
}

void	resetRequestResponse(request_t &request, response_t &response)
{
	logger.log(DEBUG, "Cleaning request and response struct");
	resetRequest(request);
	resetResponse(response);
}

int	checkIfDir(const std::string &pathname)
{
	struct stat	s;

	if (stat(pathname.c_str(), &s) == 0)
	{
		if (s.st_mode & S_IFDIR)
			return (1);
	}
	return (0);
}

int	checkIfFile(const std::string &pathname)
{
	struct stat s;
	if (stat(pathname.c_str(), &s) == 0)
	{
		if (s.st_mode & S_IFREG)
			return (1);
	}
	return (0);
}