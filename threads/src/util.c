#include "util.h"

#include <assert.h>
#include <stdlib.h>

void alloc_and_push(struct queue *q, int value) {
    struct queue_entry *entry = malloc(sizeof(struct queue_entry));
    assert(entry != NULL);

    entry->value = value;

    queue_push(q, entry);

    debug_println("producer", "push %d", value);
}
