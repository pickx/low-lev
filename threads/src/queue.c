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
    check_ret(ret, "pthread_mutex_init");

    ret = pthread_cond_init(&q->not_empty, NULL);
    check_ret(ret, "pthread_cond_init of not_empty");

    ret = pthread_cond_init(&q->not_full, NULL);
    check_ret(ret, "pthread_cond_init of not_full");

    q->len = 0;

    assert(capacity >= 0);
    q->capacity = capacity;

    q->disabled = false;

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
    int ret;

    ret = pthread_mutex_destroy(&q->lock);
    check_ret(ret, "pthread_mutex_destroy");

    ret = pthread_cond_destroy(&q->not_empty);
    check_ret(ret, "pthread_cond_destroy (not_empty)");

    ret = pthread_cond_destroy(&q->not_full);
    check_ret(ret, "pthread_cond_destroy (not_full)");

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

    // can assert that this is true when
    // entering this function, but that just
    // adds extra steps to `queue_destroy`
    q->disabled = true;
}

void queue_disable(struct queue *q) {
    pthread_mutex_lock(&q->lock);

    assert(!q->disabled);
    q->disabled = true;

    // gotta wake up any threads sleeping in `queue_pop`
    pthread_cond_broadcast(&q->not_empty);

    pthread_mutex_unlock(&q->lock);
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
        check_ret(ret, "pthread_cond_wait of not_full");
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
    check_ret(ret, "pthread_cond_signal of not_empty");

    // as the signature of `pthread_cond_signal` shows,
    // signalling does not unlock the mutex.
    // it is only on releasing the mutex,
    // that something waiting on mutex _might_ wake up
    // from the signal above
    pthread_mutex_unlock(&q->lock);
}

// returns NULL iff the queue is inactive
struct queue_entry *queue_pop(struct queue *q) {
    pthread_mutex_lock(&q->lock);

    if (q->disabled) {
        pthread_mutex_unlock(&q->lock);
        return NULL;
    }

    struct queue_entry *entry = NULL;

    int ret;

    while (queue_is_empty(q)) {
        debug_println("queue_pop", "waiting (queue empty)");
        
        // this releases the mutex...
        ret = pthread_cond_wait(&q->not_empty, &q->lock);
        check_ret(ret, "pthread_cond_wait of not_empty");

        // ...and now that we're back here,
        // we have the mutex again.
        // however, we still need to re-check for NULL,
        // (hence this is a `while` loop), and this is because:
        // 1. race condition with other calls to this function
        // 2. spurious wakeups
    }

    // we need to check _again_ in case we were
    // woken up by `queue_disable`.
    // and we can't combine this with the above
    // `disabled` check, because it can
    // cause indefinite sleep
    if (q->disabled) {
        pthread_mutex_unlock(&q->lock);
        return NULL;
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
    check_ret(ret, "pthread_cond_signal of not_full");
    if (q->len == q->capacity - 1) {
        ret = pthread_cond_signal(&q->not_full);
        check_ret(ret, "pthread_cond_signal of not_full");
    }
    
    pthread_mutex_unlock(&q->lock);

    return entry;
}
