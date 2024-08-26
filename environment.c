#include "shell.h"

/**
 * _getenv - Get the value of an environment variable
 * @name: Name of the environment variable
 *
 * Return: Value of the environment variable, or NULL if not found
 */
char *_getenv(char *name)
{
	int i = 0;
	char *token, *dup, *value = NULL;

	while (environ[i])
	{
		dup = strdup(environ[i]);
		token = strtok(dup, "=");
		if (!strcmp(token, name))
		{
			token = strtok(NULL, "=");
			if (token)
				value = strdup(token);
			free(dup);
			return (value);
		}
		free(dup);
		i++;
	}
	return (NULL);
}
