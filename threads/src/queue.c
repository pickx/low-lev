#include "queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// SAFETY: this is non-reentrant.
void queue_init(struct queue *q) {
    int ret = pthread_mutex_init(&q->lock, NULL);
    if (ret != 0) {
        fprintf(stderr, "pthread_mutex_init: %s\n", strerror(ret));
        exit(EXIT_FAILURE);
    }

    q->head = NULL;
    q->tail = NULL;
}

// SAFETY: this is non-reentrant,
// and caller asserts that:
// 1. `q` is init
// 2. `q` has not been destroyed
// 3. `q.lock` is not in use
void queue_destroy(struct queue *q, bool free_list_entries) {
    int ret = pthread_mutex_destroy(&q->lock);
    if (ret != 0) {
        fprintf(stderr, "pthread_mutex_destroy: %s\n", strerror(ret));
        exit(EXIT_FAILURE);
    }

    if (free_list_entries) {
        struct queue_entry *entry = q->head;
        while (entry != NULL) {
            struct queue_entry *next = entry->next;
            free(entry);
            entry = next;
        }
    }
}

void queue_push(struct queue *q, struct queue_entry *entry) {
    if (entry == NULL) {
        // maybe an assert makes more sense here
        return;
    }

    entry->next = NULL;

    pthread_mutex_lock(&q->lock);

    if (q->tail == NULL) {
        q->tail = entry;
        q->head = q->tail;
    } else {
        q->tail->next = entry;
        q->tail = q->tail->next;
    }

    pthread_mutex_unlock(&q->lock);
}

struct queue_entry *queue_pop(struct queue *q) {
    pthread_mutex_lock(&q->lock);

    struct queue_entry *entry = NULL;

    if (q->head != NULL) {
        entry = q->head;

        q->head = q->head->next;
        if (q->head == NULL) {
            q->tail = NULL;
        }

        // must be done after advancing head,
        // ...else we're mutating the list.
        entry->next = NULL;
    }

    pthread_mutex_unlock(&q->lock);

    return entry;
}
