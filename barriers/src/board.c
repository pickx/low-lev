#include "board.h"
#include "../../shared/shared.h"

#include <stdio.h>
#include <stdlib.h>

int len(const struct Dimensions *dim) { return dim->rows * dim->cols; }

int to_idx(const struct Dimensions *dim, int row, int col) {
    return (row * dim->cols) + col;
}

static bool parse_dim(char *arg, int *dim) {
    if (!parse_int(arg, dim)) {
        fprintf(stderr, "bad arg: %s\n", arg);
        return false;
    }

    static const int MIN_DIM = 1;
    static const int MAX_DIM = 25;

    if (MIN_DIM <= *dim && *dim <= MAX_DIM) {
        return true;
    } else {
        fprintf(
            stderr,
            "invalid dimension: %d (valid range: %d..%d)\n",
            *dim,
            MIN_DIM,
            MAX_DIM
        );
        return false;
    }
}

bool init_from_args(struct Board *board, char *arg1, char *arg2) {
    board->curr = NULL;
    board->next = NULL;

    if (!parse_dim(arg1, &board->dim.rows)) {
        return false;
    }
    if (!parse_dim(arg2, &board->dim.cols)) {
        return false;
    }

    size_t size = len(&board->dim) * sizeof *board->curr;

    board->curr = malloc(size);
    if (board->curr == NULL) {
        return false;
    }

    board->next = malloc(size);
    if (board->next == NULL) {
        free_board(board);
        return false;
    }

    return true;
}

void free_board(struct Board *board) {
    free(board->curr);
    free(board->next);
    board->curr = NULL;
    board->next = NULL;
}

void seed_board(struct Board *board, const struct Pos *seed, size_t seed_len) {
    for (int i = 0; i < len(&board->dim); i += 1) {
        board->curr[i] = false;
        board->next[i] = false;
    }

    struct Dimensions *dim = &board->dim;

    for (size_t i = 0; i < seed_len; i += 1) {
        int row = seed[i].row;
        int col = seed[i].col;

        if (row < 0 || row >= dim->rows || col < 0 || col >= dim->cols) {
            fprintf(
                stderr,
                "got out-of-bounds seed: (%d, %d) for %dx%d board\n",
                row,
                col,
                dim->rows,
                dim->cols
            );
            exit(EXIT_FAILURE);
        }

        int idx = to_idx(&board->dim, row, col);

        board->curr[idx] = true;
    }
}

const struct Pos column[10] = {
    {.row = 8, .col = 3},
    {.row = 8, .col = 4},
    {.row = 8, .col = 5},
    {.row = 8, .col = 6},
    {.row = 8, .col = 7},
    {.row = 8, .col = 8},
    {.row = 8, .col = 9},
    {.row = 8, .col = 10},
    {.row = 8, .col = 11},
    {.row = 8, .col = 12},
};

const struct Pos glider[5] = {
    {.row = 1, .col = 2},
    {.row = 2, .col = 3},
    {.row = 3, .col = 1},
    {.row = 3, .col = 2},
    {.row = 3, .col = 3},
};
