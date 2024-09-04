#include "../../include/Webserv.hpp"

size_t	ft_strlen(const char *s)
{
	int	i;

	i = 0;
	if (!s || !s[0])
		return (0);
	while (s[i] != '\0')
		++i;
	return (i);
}

char *ft_read(size_t fd, char buffer[BUFFERSIZE], int *bytes_read)
{
    char primary[BUFFERSIZE];
    int primBytes;

    primBytes = read(fd, primary, BUFFERSIZE);
    if (primBytes == -1)
        return NULL;

    if (primBytes > 0)
    {
        strncpy(buffer, primary, primBytes);
        if (primBytes < BUFFERSIZE)
            buffer[primBytes] = '\0';
        else
            buffer[BUFFERSIZE - 1] = '\0';

        *bytes_read = primBytes;
        return buffer;
    }
    *bytes_read = 0;
    buffer[0] = '\0';
    return buffer;
}

char	*ft_strjoin(char const *s1, char const *s2)
{
	char	*newarray;
	int		i;
	int		j;

	newarray = (char *)malloc(ft_strlen(s1) + ft_strlen(s2) + 1);
	i = 0;
	if (newarray == NULL)
		return (NULL);
	while (s1[i] != '\0')
	{
		newarray[i] = s1[i];
		i++;
	}
	j = i;
	i = 0;
	while (s2[i] != '\0')
	{
		newarray[j] = s2[i];
		i++;
		j++;
	}
	newarray[ft_strlen(s1) + ft_strlen(s2)] = '\0';
	return (newarray);
}
