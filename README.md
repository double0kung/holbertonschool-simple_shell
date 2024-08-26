# Simple Shell Project

This is a simple UNIX command line interpreter created as part of the Holberton School curriculum.

## Compilation

To compile the shell, use:

gcc -Wall -Werror -Wextra -pedantic -std=gnu89 *.c -o hsh

## Usage

The shell can be run in interactive mode:

./hsh

Or in non-interactive mode:

echo "/bin/ls" | ./hsh

## Features

- Displays a prompt and waits for user input
- Executes commands with their arguments
- Handles the PATH
- Implements built-in commands: exit, env
- Handles errors

## Authors

- Volodymyr Gozhyi
- David Zajicek
- Sonny Keo