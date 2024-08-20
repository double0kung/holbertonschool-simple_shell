#include "shell.h"

/**
 * main - Entry point for the shell
 * @ac: Argument count (unused)
 * @av: Argument vector (unused)
 * @env: Environment variables (unused)
 *
 * Return: Always 0
 */
int main(int ac, char **av, char **env)
{
	(void)ac;
	(void)av;
	(void)env;

	if (isatty(STDIN_FILENO))
	{
		shell_interactive_mode();
	}
	else
	{
		shell_non_interactive_mode();
	}
	return (0);
}
