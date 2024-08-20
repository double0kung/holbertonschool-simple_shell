
#include "shell.h"

void shell_interactive_mode()
{
	int status = -1;
	char *line;
	char **args;
	do
	{
		printf("$");
		line = read_line();
		// split string into args or tokens
		// check and execute args
		//args = split_line(line);
		//status = execute_arguments(args);
		printf("%s\n", line);
				
		free(line);
		//free(args);

		if (status >= 0)
			exit(status);
	}while (status == -1);
}

void shell_non_interactive_mode()
{
	int status = -1;
	char *line;
	char **args;

	printf("before main loop, non-interactive\n");
	// similar to interactive, do while loop,
	// read in line froms stream, split line, execute args.
	do
	{
		line = read_stream();
		printf("%s\n", line);
		free(line);
		//free(args);
		if (status >= 0)
		{
			exit(status);
		}
	} while (status == -1);
}

char* read_line()
{
	char *line = NULL;
	size_t bufsize = 0;

	if (getline(&line, &bufsize, stdin) == -1)
		{
			if (feof(stdin))
			{
				free(line);
				printf("end of file reached\n");
				exit(EXIT_SUCCESS);
			}
			else
			{
				free(line);
				printf("error reading line from stdin");
				exit(EXIT_FAILURE);
			}
		}
	return (line);
}

char *read_stream()
{
	size_t bufsize = 0;
	char *line = NULL;

	printf("reading stream\n");
	if (getline(&line, &bufsize, stdin) == -1)
		{
			if (feof(stdin))
			{
				free(line);
				printf("reached end of file, non-interactive");
				exit(EXIT_SUCCESS);
			}
			else
			{
				free(line);
				printf("error reading line from stdin, non-interactive\n");
				exit(EXIT_FAILURE);
			}
		}
	return (line);
}

int execute_arguments(char **args)
{
	int i = 0;

	if (!args[0])
	{
		return (-1);
	}

	
}
