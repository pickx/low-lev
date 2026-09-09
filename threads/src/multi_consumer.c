#include "queue.h"
#include "util.h"

#include <assert.h>
#include <fcntl.h>
#include <limits.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

static void *producer_func(void *arg) {
    struct queue *q = arg;

    const int push_limit = 500;
    const int value_limit = 5000;

    int count = 1 + (rand() % push_limit);

    for (int i = 0; i < count; i += 1) {
        int value = 1 + (rand() % value_limit);

        alloc_and_push(q, value);
    }

    queue_disable(q);

    return NULL;
}

struct consumer_arg {
    struct queue *q;
    int fd;
};

static void *consumer_func(void *arg) {
    struct consumer_arg *consumer_arg = arg;
    struct queue *q = consumer_arg->q;
    int fd = consumer_arg->fd;

    char buf[32];

    struct queue_entry *popped;

    while (true) {
        popped = queue_pop(q);

        if (popped == NULL) {
            debug_println("consumer", "queue disabled, exiting");
            break;
        } else {
            int value = popped->value;
            free(popped);

            debug_println("consumer", "pop %d", value);

            size_t len = sprintf(buf, "%d\n", value);
            int ret = write(fd, buf, len);
            if (ret == -1) {
                perror("write");
                exit(EXIT_FAILURE);
            }
        }
    }

    return NULL;
}

int main() {
    struct queue q;

    size_t capacity = 150;
    queue_init(&q, capacity);

    int ret;

    pthread_t producer;

    int consumer_count = 4;
    pthread_t consumers[consumer_count];

    // seed random for producer, should only be called once
    srand(time(NULL));

    ret = pthread_create(&producer, NULL, producer_func, &q);
    check_ret(ret, "pthread_create (producer)");

    int fd =
        open("queue_values.txt", O_APPEND | O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    struct consumer_arg consumer_arg = {&q, fd};

    for (int i = 0; i < consumer_count; i += 1) {
        ret = pthread_create(&consumers[i], NULL, consumer_func, &consumer_arg);
        check_ret(ret, "pthread_create (consumer)");
    }

    // producer signals exit to consumer,
    // ...therefore we join on producer first.

    ret = pthread_join(producer, NULL);
    check_ret(ret, "pthread_join (producer)");

    for (int i = 0; i < consumer_count; i += 1) {
        ret = pthread_join(consumers[i], NULL);
        check_ret(ret, "pthread_join (consumer)");
    }

    ret = close(fd);
    if (ret == -1) {
        perror("close");
        exit(EXIT_FAILURE);
    }

    queue_destroy(&q, false);
}
