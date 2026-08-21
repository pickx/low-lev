#include "helpers.h"

int parse_int(const char *s, int *out)
{
    if (s == NULL || *s == '\0') {
        return -1;
    }

    char *endptr;
    errno = 0;
    long val = strtol(s, &endptr, 10);

    if (errno != 0 || endptr == s || *endptr != '\0') {
        return -1;
    }

    if (val < INT_MIN || val > INT_MAX) {
        return -1;
    }

    *out = (int)val;
    return 0;
}

bool is_exit_input(const char *s)
{
    while (isspace((unsigned char)*s)) {
        s++;
    }

    if (*s != 'e') {
        return false;
    }

    s++;

    while (isspace((unsigned char)*s)) {
        s++;
    }

    return *s == '\0';
}

void try_signal(pid_t pid, int signal) {
    if (kill(pid, signal) == -1) {
        fprintf(stderr, "cannot signal pid %d: %s\n", pid, strerror(errno));
        exit(1);
    }
}
