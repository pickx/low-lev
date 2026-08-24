#define _POSIX_C_SOURCE 200112L

#include "shared.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static void fail_ret_message(int ret, const char *format, va_list args) {
    vfprintf(stderr, format, args);
    fprintf(stderr, ": %s\n", strerror(ret));
}

void check_ret_nonzero(int ret, const char *format, ...) {
    if (ret != 0) {
        va_list args;

        va_start(args, format);
        fail_ret_message(ret, format, args);
        va_end(args);

        exit(EXIT_FAILURE);
    }
}

#if defined(_POSIX_BARRIERS) && _POSIX_BARRIERS > 0

#include <pthread.h>
void check_ret_barrier_wait(int ret, const char *format, ...) {
    if (ret != 0 && ret != PTHREAD_BARRIER_SERIAL_THREAD) {
        va_list args;

        va_start(args, format);
        fail_ret_message(ret, format, args);
        va_end(args);

        exit(EXIT_FAILURE);
    }
}

#endif
