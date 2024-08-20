#ifndef SHELL_H
#define SHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

extern char **environ;

void shell_interactive_mode();

void shell_non_interactive_mode();

char *read_line();

char *read_stream();

char *split_string(char* string);

int execute_arguments(char** args);

#endif
