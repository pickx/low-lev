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

#endif
