#include "shell.h"


int main (int ac, char **av, char** env)
{
	if (isatty(STDIN_FILENO))
	{
		printf("Entered interactive mode\n");
		shell_interactive_mode();
	}
	else
	{
		printf("Entered non-interactive mode\n");
		shell_non_interactive_mode();
	}
	
	return (0);
}
