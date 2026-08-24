#include "util.h"

bool is_exit_input(const char *s) {
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
