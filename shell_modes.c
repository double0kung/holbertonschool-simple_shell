#include "shell.h"

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
