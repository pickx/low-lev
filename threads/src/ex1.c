#include "queue.h"
#include <limits.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef DEBUG
#define debug_printf(...)                                                      \
    do {                                                                       \
        printf(__VA_ARGS__);                                                   \
        fflush(stdout);                                                        \
    } while (0)
#else
#define debug_printf(...) ((void)0)
#endif

static void alloc_and_push(struct queue *q, int value) {
    struct queue_entry *entry = malloc(sizeof(struct queue_entry));
    entry->value = value;

    queue_push(q, entry);

    debug_printf("PRODUCER: pushed %d\n", value);
}

static void *producer_func(void *arg) {
    struct queue *q = arg;

    const int push_limit = 500;
    const int value_limit = 5000;

    int count = 1 + (rand() % push_limit);

    for (int i = 0; i < count; i += 1) {
        int value = 1 + (rand() % value_limit);

        alloc_and_push(q, value);
    }

    alloc_and_push(q, 0);

    return NULL;
}

static void *consumer_func(void *arg) {
    struct queue *q = arg;

    struct queue_entry *popped;
    struct timespec cooldown = {0, 50000000};

    FILE *file = fopen("queue_values.txt", "w");
    if (file == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    while (true) {
        popped = queue_pop(q);

        if (popped == NULL) {
            debug_printf("CONSUMER: empty queue, sleeping\n");
            nanosleep(&cooldown, NULL); // not checking return value here
        } else {
            int value = popped->value;
            free(popped);

            debug_printf("CONSUMER: popped %d\n", value);

            if (value == 0) {
                break;
            } else {
                fprintf(file, "%d\n", value);
            }
        }
    }

    fclose(file); // not checking return value here

    return NULL;
}

static void check_ret(int ret, const char *func_name) {
    if (ret != 0) {
        fprintf(stderr, "%s: %s\n", func_name, strerror(ret));
        exit(EXIT_FAILURE);
    }
}

int main() {
    struct queue q;

    queue_init(&q);

    int ret;

    pthread_t producer;
    pthread_t consumer;

    // seed random for producer, should only be called once
    srand(time(NULL));

    ret = pthread_create(&producer, NULL, producer_func, &q);
    check_ret(ret, "pthread_create (producer)");

    ret = pthread_create(&consumer, NULL, consumer_func, &q);
    check_ret(ret, "pthread_create (consumer)");

    // producer signals exit to consumer,
    // ...therefore we join on producer first.

    ret = pthread_join(producer, NULL);
    check_ret(ret, "pthread_join (producer)");

    ret = pthread_join(consumer, NULL);
    check_ret(ret, "pthread_join (consumer)");

    queue_destroy(&q, false);
}
