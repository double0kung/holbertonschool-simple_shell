#include "shell.h"
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

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

/**
 * shell_interactive_mode - Runs the shell in interactive mode
 */
void shell_interactive_mode(void)
{
	char *line;
	char **args;
	int status;

	do {
		printf(":) ");
		fflush(stdout);
		line = read_line();
		if (line == NULL)
			break;
		args = split_line(line);
		status = execute_command(args);

		free(line);
		free(args);
	} while (status == -1);

	if (status >= 0)
		exit(status);
}

/**
 * shell_non_interactive_mode - Run the shell in non-interactive mode
 */
void shell_non_interactive_mode(void)
{
	char *line;
	char **args;
	int status = 0;

	while ((line = read_stream()) != NULL)
	{
		args = split_line(line);
		status = execute_command(args);

		free(line);
		free(args);

		if (status >= 0)
			break;
	}
	exit(status);
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

	if (position == 0)
	{
		tokens[0] = "";
		tokens[1] = NULL;
	}

	return (tokens);
}

/**
 * find_command - Find the full path of a command
 * @command: Command to find
 *
 * Return: Full path of the command, or NULL if not found
 */
char *find_command(char *command)
{
	char *path, *path_copy, *file_path, *token;
	struct stat buf;

	path = _getenv("PATH");
	if (!path)
		return (NULL);
	path_copy = strdup(path);
	token = strtok(path_copy, ":");
	while (token != NULL)
	{
		file_path = malloc(strlen(token) + strlen(command) + 2);
		snprintf(file_path, strlen(token) + strlen(command) + 2,
			 "%s/%s", token, command);
		if (stat(file_path, &buf) == 0)
		{
			free(path_copy);
			free(path);
			return (file_path);
		}
		free(file_path);
		token = strtok(NULL, ":");
	}
	free(path_copy);
	free(path);
	return (NULL);
}

/**
 * fork_and_exec - Fork a new process and execute a command
 * @command: Command to execute
 * @args: Array of command arguments
 *
 * Return: Exit status of the command, or -1 on error
 */
int fork_and_exec(char *command, char **args)
{
	pid_t pid;
	int status;

	pid = fork();
	if (pid == 0)
	{
		/* Child process */
		if (execve(command, args, environ) == -1)
		{
			/* If execve fails, exit with error code */
			exit(errno == ENOENT ? 127 : 126);
		}
	}
	else if (pid < 0)
	{
		/* Fork failed */
		perror("fork");
		return (-1);
	}
	else
	{
		/* Parent process */
		do {
			waitpid(pid, &status, WUNTRACED);
		} while (!WIFEXITED(status) && !WIFSIGNALED(status));

		if (WIFEXITED(status))
			return (WEXITSTATUS(status));
	}

	return (-1);
}

/**
 * execute_command - Execute a command
 * @args: Array of command arguments
 *
 * Return: Exit status of the command
 */
int execute_command(char **args)
{
	int status;

	if (args[0] == NULL || args[0][0] == '\0')
		return (-1);

	/* Check for built-in commands first */
	if (strcmp(args[0], "env") == 0)
		return (own_env(args));
	if (strcmp(args[0], "exit") == 0)
		return (own_exit(args));

	/* For external commands, always try to execute */
	status = fork_and_exec(args[0], args);

	if (status == -1)
	{
		/* Command execution failed */
		if (errno == ENOENT)
			fprintf(stderr, "./hsh: 1: %s: not found\n", args[0]);
		else
			fprintf(stderr, "./hsh: 1: %s: Permission denied\n", args[0]);
		return (127);  /* Return 127 for command not found or permission denied */
	}

	return (status);
}

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
