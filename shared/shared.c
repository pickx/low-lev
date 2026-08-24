#include "shared.h"

#include <errno.h>
#include <limits.h>
#include <stdlib.h>

bool parse_int(const char *s, int *out) {
    if (s == NULL || *s == '\0') {
        return false;
    }

    char *endptr;
    errno = 0;
    long val = strtol(s, &endptr, 10);

    if (errno != 0
        || endptr == s
        || *endptr != '\0'
        || val < INT_MIN
        || val > INT_MAX) {
        return false;
    }

    *out = (int)val;
    return true;
}
