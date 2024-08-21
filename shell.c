
#include "shell.h"
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

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

		if (status >= 0)
		{
			exit(status);
		}
	} while (status == -1);  /* -1 could be your exit condition */
}

/**
 * shell_non_interactive_mode - Runs the shell in non-interactive mode
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
 * execute_command - Executes a command
 * @args: Array of arguments
 *
 * Return: 1 if the shell should continue running, 0 if it should terminate
 */
int execute_command(char **args)
{
	pid_t pid;
	int status;
	long unsigned int i = 0;
	char *path, *token;
	char cmd_full_path[1024];
	char *builtin_func_list[] = {
		"env",
		"exit"
	};
	int  (*builtin_func[])(char**) = {
		&own_env,
		&own_exit
	};

	if (args[0] == NULL || args[0][0] == '\0')
		return (-1);  /* Return 0 for empty or whitespace-only commands */
	while (i < sizeof(builtin_func_list) / sizeof(char *))
	{
		if (strcmp(args[0], builtin_func_list[i]) == 0)
		{
			return ((*builtin_func[i])(args));
		}
		i++;
	}

	pid = fork();
	if (pid == 0)
	{
		/* Child process */
		if (strchr(args[0], '/') != NULL)
		{
			/* If command includes a path, execute it directly */
			execve(args[0], args, environ);
		}
		else
		{
			
			/* If not, search in PATH */
			path = getenv("PATH");
			token = strtok(path, ":");
			while (token != NULL)
			{
				snprintf(cmd_full_path, sizeof(cmd_full_path), "%s/%s", token, args[0]);
				execve(cmd_full_path, args, environ);
				token = strtok(NULL, ":");
			}
		}
		/* If we get here, the command was not found */
		fprintf(stderr, "%s: command not found\n", args[0]);
		exit(127); /* 127 is the standard exit code for "command not found" */
	}
	else if (pid < 0)
	{
		/* Error forking */
		perror("hsh");
		return (1);
	}
	else
	{
		/* Parent process */
		do {
			waitpid(pid, &status, WUNTRACED);
		} while (!WIFEXITED(status) && !WIFSIGNALED(status));

		if (WIFEXITED(status))
		{
			return (-1);
		}
		else
		{
			return (1);
		}
	}
}

int own_exit(char **args)
{
	(void)(**args);

	return (0);
}

int own_env(char *

*args)
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
