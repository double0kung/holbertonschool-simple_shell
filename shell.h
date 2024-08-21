#ifndef SHELL_H
#define SHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

extern char **environ;

void shell_interactive_mode(void);
void shell_non_interactive_mode(void);
char *read_line(void);
char *read_stream(void);
char **split_line(char *line);
int execute_command(char **args);

int own_exit(char **args);
int own_env();

#endif /* SHELL_H */
