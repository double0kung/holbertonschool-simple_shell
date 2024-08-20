#include "shell.h"
#include <string.h>
#include <sys/wait.h>

/**
 * shell_interactive_mode - Runs the shell in interactive mode
 */
void shell_interactive_mode(void)
{
	char *line;
	char **args;
	int status;

	do {
		printf("$ ");
		line = read_line();
		args = split_line(line);
		status = execute_command(args);

		free(line);
		free(args);
	} while (status);
}

/**
 * shell_non_interactive_mode - Runs the shell in non-interactive mode
 */
void shell_non_interactive_mode(void)
{
	char *line;
	char **args;
	int status;

	while ((line = read_stream()) != NULL)
	{
		args = split_line(line);
		status = execute_command(args);

		free(line);
		free(args);
		if (status >= 0)
		{
			exit(status);
		}
	}
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
	ssize_t characters;

	characters = getline(&line, &bufsize, stdin);
	if (characters == -1)
	{
		free(line);
		exit(EXIT_SUCCESS);
	}
	return (line);
}

/**
 * read_stream - Reads a line from stdin in non-interactive mode
 *
 * Return: Pointer to the read line, or NULL if EOF is reached
 */
char *read_stream(void)
{
	char *line = NULL;
	size_t bufsize = 0;
	ssize_t characters;

	characters = getline(&line, &bufsize, stdin);
	if (characters == -1)
	{
		free(line);
		return (NULL);
	}
	return (line);
}

/**
 * split_line - Splits a line into tokens
 * @line: The line to split
 *
 * Return: Array of tokens
 */
char **split_line(char *line)
{
	int bufsize = 64, position = 0;
	char **tokens = malloc(bufsize * sizeof(char *));
	char *token;

	if (!tokens)
	{
		fprintf(stderr, "allocation error\n");
		exit(EXIT_FAILURE);
	}

	token = strtok(line, " \t\r\n\a");
	while (token != NULL)
	{
		tokens[position] = token;
		position++;

		if (position >= bufsize)
		{
			bufsize += 64;
			tokens = realloc(tokens, bufsize * sizeof(char *));
			if (!tokens)
			{
				fprintf(stderr, "allocation error\n");
				exit(EXIT_FAILURE);
			}
		}

		token = strtok(NULL, " \t\r\n\a");
	}
	tokens[position] = NULL;
	return (tokens);
}

/**
 * execute_command - Executes a command
 * @args: Array of arguments
 *
 * Return: 1 if the shell should continue running, 0 if it should terminate
 */
int execute_command(char **args)
{
	pid_t pid;
	int status;

	if (args[0] == NULL)
	{
		return (1);
	}

	pid = fork();
	if (pid == 0)
	{
		/* Child process */
		if (execve(args[0], args, environ) == -1)
		{
			perror("hsh");
		}
		exit(EXIT_FAILURE);
	}
	else if (pid < 0)
	{
		/* Error forking */
		perror("hsh");
	}
	else
	{
		/* Parent process */
		do {
			waitpid(pid, &status, WUNTRACED);
		} while (!WIFEXITED(status) && !WIFSIGNALED(status));
	}

	return (1);
}
