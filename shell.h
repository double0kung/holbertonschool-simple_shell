#ifndef SHELL_H
#define SHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <errno.h>

extern char **environ;

/* Function prototypes */
void shell_interactive_mode(void);
void shell_non_interactive_mode(void);
char *read_line(void);
char *read_stream(void);
char **split_line(char *line);
int execute_command(char **args);
char *find_command(char *command);
char *_getenv(char *name);
int fork_and_exec(char *command, char **args);

/* Builtin function prototypes */
int own_exit(char **args);
int own_env(char **args);

#endif /* SHELL_H */
