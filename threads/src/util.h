#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>

// XXX: this is not concurrent,
// but we call it from the multi-consumer program.
// might fix later.
#define debug_println(sender, ...)                                             \
    do {                                                                       \
        printf("%s: ", sender);                                                \
        printf(__VA_ARGS__);                                                   \
        printf("\n");                                                          \
        fflush(stdout);                                                        \
    } while (0)

void check_ret(int ret, const char *desc);
void alloc_and_push(struct queue *q, int value);

#endif
