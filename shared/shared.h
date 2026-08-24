#ifndef SHARED_H
#define SHARED_H

#include <stdbool.h>

bool parse_int(const char *s, int *out);

void check_ret_nonzero(int ret, const char *format, ...);
void check_ret_barrier_wait(int ret, const char *format, ...);

#endif
