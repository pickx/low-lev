#ifndef BOARD_H
#define BOARD_H

#include <stdbool.h>
#include <stddef.h>

// should probably use size_t here (and in other places)
struct Dimensions {
    int rows;
    int cols;
};

struct Board {
    bool *curr;
    bool *next;
    struct Dimensions dim;
};

struct Pos {
    int row;
    int col;
};

int len(const struct Dimensions *dim);
int to_idx(const struct Dimensions *dim, int row, int col);
int to_row(const struct Dimensions *dim, int idx);
int to_col(const struct Dimensions *dim, int idx);

bool init_from_args(struct Board *board, char *arg1, char *arg2);
void free_board(struct Board *board);
void seed_board(struct Board *board, const struct Pos *seed, size_t seed_len);

// TODO: add more examples

// expects 17x17, 20 generations
extern const struct Pos column[10];

// expects 12x12, 15 generations
extern const struct Pos glider[5];

#endif
