#include "queue.h"
#include "util.h"
#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// SAFETY: this is non-reentrant.
void queue_init(struct queue *q, size_t capacity) {
    int ret;

    ret = pthread_mutex_init(&q->lock, NULL);
    if (ret != 0) {
        fprintf(stderr, "pthread_mutex_init: %s\n", strerror(ret));
        exit(EXIT_FAILURE);
    }

    ret = pthread_cond_init(&q->not_empty, NULL);
    if (ret != 0) {
        fprintf(stderr, "pthread_cond_init of not_empty: %s\n", strerror(ret));
        exit(EXIT_FAILURE);
    }

    ret = pthread_cond_init(&q->not_full, NULL);
    if (ret != 0) {
        fprintf(stderr, "pthread_cond_init of not_full: %s\n", strerror(ret));
        exit(EXIT_FAILURE);
    }

    q->len = 0;

    assert(capacity >= 0);
    q->capacity = capacity;

    q->head = NULL;
    q->tail = NULL;
}

bool queue_is_empty(struct queue *q) {
    return q->len == 0;
}

bool queue_is_full(struct queue *q) {
    return q->len >= q->capacity;
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

    q->head = NULL;
    q->tail = NULL;
    q->len = 0;
}

void queue_push(struct queue *q, struct queue_entry *entry) {
    assert(entry != NULL);
    entry->next = NULL;

    pthread_mutex_lock(&q->lock);

    int ret;

    while (queue_is_full(q)) {
        debug_println("queue_push", "waiting (queue full)");

        // same comments as the `pthread_cond_wait` in `queue_pop`
        ret = pthread_cond_wait(&q->not_full, &q->lock);
        if (ret != 0) {
            fprintf(stderr, "pthread_cond_wait of not_full: %s\n", strerror(ret));
            exit(EXIT_FAILURE);
        }
    }

    if (queue_is_empty(q)) {
        q->tail = entry;
        q->head = q->tail;
    } else {
        q->tail->next = entry;
        q->tail = entry;
    }

    q->len += 1;

    ret = pthread_cond_signal(&q->not_empty);
    if (ret != 0) {
        fprintf(stderr, "pthread_cond_signal of not_empty: %s\n", strerror(ret));
        exit(EXIT_FAILURE);
    }

    // as the signature of `pthread_cond_signal` shows,
    // signalling does not unlock the mutex.
    // it is only on releasing the mutex,
    // that something waiting on mutex _might_ wake up
    // from the signal above
    pthread_mutex_unlock(&q->lock);
}

struct queue_entry *queue_pop(struct queue *q) {
    pthread_mutex_lock(&q->lock);

    struct queue_entry *entry = NULL;

    int ret;

    while (queue_is_empty(q)) {
        debug_println("queue_pop", "waiting (queue empty)");
        
        // this releases the mutex...
        ret = pthread_cond_wait(&q->not_empty, &q->lock);
        if (ret != 0) {
            fprintf(stderr, "pthread_cond_wait of not_empty: %s\n", strerror(ret));
            exit(EXIT_FAILURE);
        }        
        // ...and now that we're back here,
        // we have the mutex again.
        // however, we still need to re-check for NULL,
        // (hence this is a `while` loop), and this is because:
        // 1. race condition with other calls to this function
        // 2. spurious wakeups
    }

    entry = q->head;

    q->head = q->head->next;
    if (q->head == NULL) {
        q->tail = NULL;
    }

    // must be done after advancing head,
    // ...else we're mutating the list.
    entry->next = NULL;

    q->len -= 1;

    ret = pthread_cond_signal(&q->not_full);
    if (ret != 0) {
        fprintf(stderr, "pthread_cond_signal of not_full: %s\n", strerror(ret));
        exit(EXIT_FAILURE);
    }

    pthread_mutex_unlock(&q->lock);

    return entry;
}
