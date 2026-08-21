#define _POSIX_C_SOURCE 200809L

#include "helpers.h"
#include <stdlib.h>
#include <sys/resource.h>

int main(int argc, char *argv[])
{

    if (argc != 2) {
        fprintf(stderr, "usage: %s <pid>\n", argv[0]);
        return 1;
    }

    int pid;
    if (!parse_int(argv[1], &pid) || pid <= 0) {
        fprintf(stderr, "invalid pid: %s\n", argv[1]);
        return 1;
    }

    try_signal(pid, 0);

    int low = SIGRTMIN;
    int high = low + 2;

    char buf[32];
        
    int count;
    int signal;
    char tail[2]; // handles trailing whitespace in `sscanf`

    struct rlimit rlim;
    if (getrlimit(RLIMIT_SIGPENDING, &rlim) == -1) {
        perror("getrlimit");
        exit(EXIT_FAILURE);
    }

    while (1) {
        printf("count and signal (%d - %d)? signal queue limit is %lu: ", low, high, rlim.rlim_cur);

        if (fgets(buf, sizeof buf, stdin) == NULL) {
            return 0;
        }

        int input_end = strcspn(buf, "\n");
        buf[input_end] = '\0';

        if (is_exit_input(buf)) {
            exit(EXIT_SUCCESS);
        }

        int arg_count = sscanf(buf, "%d %d %1s", &count, &signal, tail);
        if (
            arg_count == 2
            && count >= 0
            && (low <= signal && signal <= high)
        ) {
            for (int i = 0; i < count; i += 1) {
                try_signal(pid, signal);
            }

        } else {
            printf("invalid input\n");
        }
    }
    
    return 0;
}
