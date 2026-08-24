#include "util.h"

int main() {
    int count = 0;

    sigset_t mask;

    sigemptyset(&mask);
    for (int i = 0; i < 3; i += 1) {
        sigaddset(&mask, SIGRTMIN + i);
    }

    sigprocmask(SIG_BLOCK, &mask, NULL);

    while (true) {
        int signal;

        int ret = sigwait(&mask, &signal);
        if (ret != 0) {
            fprintf(stderr, "sigwait: %s\n", strerror(ret));
            exit(EXIT_FAILURE);
        }

        if (signal == SIGRTMIN) {
            count += 1;
        } else if (signal == SIGRTMIN + 1) {
            count -= 1;
        } else if (signal == SIGRTMIN + 2) {
            printf("%d\n", count);
        } else {
            perror("got unexpected signal");
            exit(EXIT_FAILURE);
        }
    }
}
