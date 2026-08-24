#ifndef CELL_H
#define CELL_H

#include "board.h"

#include <pthread.h>
#include <stdbool.h>

struct CellArg {
    int row;
    int col;
    struct Board *board;
    pthread_barrier_t *barrier;
};

bool is_emitter(struct CellArg *c_arg);
void emit(struct CellArg *c_arg, int generation);
int self_idx(struct CellArg *c_arg);
int alive_neighbors(struct CellArg *c_arg);

#endif
