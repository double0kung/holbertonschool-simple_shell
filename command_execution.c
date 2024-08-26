#include "shell.h"

/**
 * execute_command - Execute a command
 * @args: Array of command arguments
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
		return (-1);
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
			return (127);
		}
		i = fork_and_exec(command_path, args);
		free(command_path);
		return (i);
	}
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
		exit(127);
	}
	else if (pid < 0)
	{
		perror("hsh");
		return (1);
	}
	do {
		waitpid(pid, &status, WUNTRACED);
	} while (!WIFEXITED(status) && !WIFSIGNALED(status));

	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	else
		return (1);
}
