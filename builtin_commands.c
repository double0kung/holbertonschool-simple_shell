#include "shell.h"

/**
 * own_exit - Built-in exit command
 * @args: Arguments (unused)
 *
 * Return: Always returns 0 to terminate the shell
 */
int own_exit(char **args)
{
	(void)(**args);
	return (0);
}

/**
 * own_env - Built-in env command to print environment variables
 * @args: Arguments (unused)
 *
 * Return: Always returns 0 after printing environment variables
 */
int own_env(char **args)
{
	int i = 0;

	(void)(**args);
	while (environ[i])
	{
		write(STDOUT_FILENO, environ[i], strlen(environ[i]));
		write(STDOUT_FILENO, "\n", 1);
		i++;
	}
	return (0);
}
