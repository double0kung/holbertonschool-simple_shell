#include "shell.h"

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
 * Return: Array of tokens, or NULL if line is empty or only whitespace
 */
char **split_line(char *line)
{
	int bufsize = 64, position = 0;
	char **tokens = malloc(bufsize * sizeof(char *));
	char *token, *trimmed_line;

	if (!tokens)
	{
		fprintf(stderr, "allocation error\n");
		exit(EXIT_FAILURE);
	}

	/* Trim leading and trailing whitespace */
	trimmed_line = line;
	while (*trimmed_line == ' ' || *trimmed_line == '\t')
		trimmed_line++;
	
	if (*trimmed_line == '\0')
	{
		free(tokens);
		return (NULL);  /* Return NULL for empty or whitespace-only input */
	}

	token = strtok(trimmed_line, " \t\r\n\a");
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
