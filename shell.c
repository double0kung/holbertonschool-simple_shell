#include "shell.h"

/**
 * shell_interactive_mode - Runs the shell in interactive mode
 */
void shell_interactive_mode(void)
{
	int status = -1;
	char *line;

	do {
		printf("$ ");
		line = read_line();
		/* split string into args or tokens */
		/* check and execute args */
		/* args = split_line(line); */
		/* status = execute_arguments(args); */
		printf("%s\n", line);

		free(line);
		if (status >= 0)
			exit(status);
	} while (status == -1);
}

/**
 * shell_non_interactive_mode - Runs the shell in non-interactive mode
 */
void shell_non_interactive_mode(void)
{
	int status = -1;
	char *line;

	do {
		line = read_stream();
		printf("%s\n", line);
		free(line);
		if (status >= 0)
		{
			exit(status);
		}
	} while (status == -1);
}

/**
 * read_line - Reads a line from stdin
 *
 * Return: Pointer to the read line
 */
char *read_line(void)
{
	char *line = NULL;
	size_t bufsize = 0;

	if (getline(&line, &bufsize, stdin) == -1)
	{
		if (feof(stdin))
		{
			free(line);
			exit(EXIT_SUCCESS);
		}
		else
		{
			free(line);
			perror("read_line");
			exit(EXIT_FAILURE);
		}
	}
	return (line);
}

/**
 * read_stream - Reads a line from stdin in non-interactive mode
 *
 * Return: Pointer to the read line
 */
char *read_stream(void)
{
	char *line = NULL;
	size_t bufsize = 0;

	if (getline(&line, &bufsize, stdin) == -1)
	{
		if (feof(stdin))
		{
			free(line);
			exit(EXIT_SUCCESS);
		}
		else
		{
			free(line);
			perror("read_stream");
			exit(EXIT_FAILURE);
		}
	}
	return (line);
}

/**
 * execute_arguments - Executes the command with given arguments
 * @args: Array of arguments
 *
 * Return: Status of execution
 */
int execute_arguments(char **args)
{
	if (!args[0])
	{
		return (-1);
	}
	/* Add command execution logic here */
	return (0);
}
