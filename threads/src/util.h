#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>

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
