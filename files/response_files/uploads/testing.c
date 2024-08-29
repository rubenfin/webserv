#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

typedef struct s_envlst
{
	char			*key; //before equal sign
	char			*value; //after equal sign
	bool			equal;
	struct s_envlst	*prev;
	struct s_envlst	*next;
}	t_envlst;

typedef struct s_parse_str
{
	char	*buffer;
	size_t	buffer_len;
	size_t	cursor_pos;
}	t_parse_str;

t_envlst	*lstnew(void)
{
	t_envlst	*envlst;

	envlst = malloc(sizeof(t_envlst));
	if (envlst == NULL)
		return (0);
	// envlst->key = NULL;
	// envlst->equal = false;
	// envlst->value = NULL;
	// envlst->prev = NULL;
	// envlst->next = NULL;
	return (envlst);
}

t_envlst	*lstlast(t_envlst *envlst)
{
	while(envlst->next != NULL)
		envlst = envlst -> next;
	return (envlst);
}

char	*get_key(t_envlst *envlst, t_parse_str *env_var)
{
	while (env_var->buffer[env_var->cursor_pos] != '=')
		env_var->cursor_pos++;
	envlst->key = malloc((sizeof(char) * env_var->cursor_pos) + 1); 
	if(envlst->key == NULL)
	{
		free(envlst);
		return (NULL);
	}
	strncpy(envlst->key, env_var->buffer, env_var->cursor_pos);
	return (envlst->key);
}

char	*get_value(t_envlst *envlst, t_parse_str env_var)
{
	int i;

	i = 0;
	while (env_var.buffer[env_var.cursor_pos] != '\0')
	{
		env_var.cursor_pos++;
		i++;
	}
	envlst->value = malloc((sizeof(t_envlst) * i) + 2);
	if (envlst->value == NULL)
	{
		free(envlst);
		return (NULL);
	}
	strncpy(envlst->value, &env_var.buffer[env_var.cursor_pos - i], i);
	strcat(envlst->value, "\n");
	return (envlst->value);
}

int	main(int argc, char **argv, char **env)
{
	int			i;
	t_parse_str	env_var;
	t_envlst 	*envlst;
	t_envlst 	*head;
	t_envlst	*last;

	i = 0;
	if (!strncmp(argv[1], "env", 4))
	{
		while (env[i])
		{
			envlst = lstnew();
			env_var.buffer = env[i];
			env_var.cursor_pos = 0;
			env_var.buffer_len = strlen(env[i]);
			envlst->key = get_key(envlst, &env_var);
			envlst->equal = (env[i][env_var.cursor_pos] == '=');
			env_var.cursor_pos++;
			envlst->value = get_value(envlst, env_var);
			if (i == 0)
			{
				head = envlst;
				head->prev = NULL;
			}
			else
			{
				last = lstlast(head);
				last->next = envlst;
				envlst->prev = last;
			}
			envlst = envlst->next;
			i++;
		}
	}
	envlst = head;
	while(envlst != NULL)
	{
		printf("%s", envlst->key);
		if(envlst->equal)
			printf("=");
		printf("%s", envlst->value);
		envlst = envlst->next;
	}
	return (0);
}

/*notes:
	- when using export it prints my env linked list in alphabetical order
	think of how to optimize it or find a way to do it
*/
