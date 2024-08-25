#include "shell.h"
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

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
		fflush(stdout);  /* Ensure the prompt is displayed */
		line = read_line();
		if (line == NULL)  /* EOF (Ctrl+D) */
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

	/* If no tokens were found, add an empty string as the first token */
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
		snprintf(file_path, strlen(token) + strlen(command) + 2, "%s/%s", token, command);
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
 * Return: Exit status of the command
 */
int fork_and_exec(char *command, char **args)
{
	pid_t pid;
	int status;

	pid = fork();
	if (pid == 0)
	{
		execve(command, args, environ);
		fprintf(stderr, "./hsh: 1: %s: not found\n", args[0]);
		exit(127);  /* Exit with 127 if execve fails */
	}
	else if (pid < 0)
	{
		perror("hsh");
		return (1);
	}
	/* Parent process */
	do {
		waitpid(pid, &status, WUNTRACED);
	} while (!WIFEXITED(status) && !WIFSIGNALED(status));

	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	else
		return (1);
}

/**
 * execute_command - Execute a command
 * @args: Array of command argument
 *
 * Return: Exit status of the command
 */
int execute_command(char **args)
{
	unsigned long int i = 0;
	char *command_path;
	char *builtin_func_list[] = {
		"env",
		"exit"
	};
	int (*builtin_func[])(char **) = {
		&own_env,
		&own_exit
	};

	if (args[0] == NULL || args[0][0] == '\0')
		return (-1);  /* Return -1 for empty or whitespace-only commands */
	while (i < sizeof(builtin_func_list) / sizeof(char *))
	{
		if (strcmp(args[0], builtin_func_list[i]) == 0)
			return ((*builtin_func[i])(args));
		i++;
	}
	if (strchr(args[0], '/') != NULL)
	{
		return (fork_and_exec(args[0], args));
	}
	else
	{
		command_path = find_command(args[0]);
		if (command_path == NULL)
		{
			fprintf(stderr, "./hsh: 1: %s: not found\n", args[0]);
			return (127);  /* Return 127 for command not found */
		}
		i = fork_and_exec(command_path, args);
		free(command_path);
		return (i);
	}
}

int own_exit(char **args)
{
	(void)(**args);

	return (0);
}

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
	return (-1);
}
