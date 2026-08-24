#ifndef UTIL_H
#define UTIL_H

#include "../../shared/shared.h"
#include "queue.h"

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

void alloc_and_push(struct queue *q, int value);

#endif
