#define _POSIX_C_SOURCE 200112L

#include "../../shared/shared.h"
#include "board.h"
#include "cell.h"

#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>

static void *cell_thread(void *arg) {
    struct CellArg *c_arg = arg;

    int idx = self_idx(c_arg);

    // TODO: make this configurable,
    // and maybe passed along with the seed
    // TODO: detect fixed point
    int generation_limit = 20;

    for (int generation = 1; generation <= generation_limit; generation += 1) {
        bool alive_curr = c_arg->board->curr[idx];
        int alive_nei = alive_neighbors(c_arg);
        bool alive_next = alive_nei == 3 || (alive_curr && alive_nei == 2);

        c_arg->board->next[idx] = alive_next;

        int ret = pthread_barrier_wait(c_arg->barrier);
        check_ret_barrier_wait(
            ret,
            "pthread_barrier_wait (1) of (%d, %d)",
            c_arg->row,
            c_arg->col
        );

        // `next` update finished.
        // now one thread emits and everyone waits on that.

        if (is_emitter(c_arg)) {
            emit(c_arg, generation);
        }

        ret = pthread_barrier_wait(c_arg->barrier);
        check_ret_barrier_wait(
            ret,
            "pthread_barrier_wait (2) of (%d, %d)",
            c_arg->row,
            c_arg->col
        );
    }

    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "usage: %s <rows> <cols>\n", argv[0]);
        return 1;
    }

    struct Board board;
    if (!init_from_args(&board, argv[1], argv[2])) {
        return 1;
    }

    seed_board(&board, column, sizeof column / sizeof column[0]);

    int cell_len = len(&board.dim);

    pthread_t threads[cell_len];
    struct CellArg c_args[cell_len];

    pthread_barrier_t barrier;
    int ret = pthread_barrier_init(&barrier, NULL, cell_len);
    check_ret_nonzero(ret, "pthread_barrier_init");

    for (int idx = 0; idx < cell_len; idx += 1) {
        int row = to_row(&board.dim, idx);
        int col = to_col(&board.dim, idx);

        c_args[idx] = (struct CellArg){
            .row = row,
            .col = col,
            .board = &board,
            .barrier = &barrier,
        };

        ret = pthread_create(&threads[idx], NULL, cell_thread, &c_args[idx]);
        check_ret_nonzero(ret, "pthread_create of (%d, %d)", row, col);
    }

    for (int idx = 0; idx < cell_len; idx += 1) {
        ret = pthread_join(threads[idx], NULL);
        check_ret_nonzero(ret, "pthread_join");
    }

    ret = pthread_barrier_destroy(&barrier);
    check_ret_nonzero(ret, "pthread_barrier_destroy");

    free_board(&board);
}
