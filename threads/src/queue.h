#ifndef QUEUE_H
#define QUEUE_H

#include <pthread.h>
#include <stdbool.h>

struct queue_entry {
    struct queue_entry *next;
    int value;
};

struct queue {
    pthread_mutex_t lock;
    struct queue_entry *head;
    struct queue_entry *tail;
};

void queue_init(struct queue *q);
void queue_destroy(struct queue *q, bool free_list_entries);
void queue_push(struct queue *q, struct queue_entry *entry);
struct queue_entry *queue_pop(struct queue *q);

#endif
