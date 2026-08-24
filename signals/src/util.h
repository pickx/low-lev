#ifndef UTIL_H
#define UTIL_H

#define _POSIX_C_SOURCE 200809L

#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

bool is_exit_input(const char *s);
void try_signal(pid_t pid, int signal);

#endif
