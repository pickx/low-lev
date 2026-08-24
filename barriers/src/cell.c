#define _POSIX_C_SOURCE 200112L

#include "cell.h"
#include <stdio.h>
#include <time.h>
#include <unistd.h>

static void clear_screen() { printf("\x1b[H\x1b[J"); }

static const int nei[8][2] = {
    {-1, -1},
    {-1, 0},
    {-1, 1},
    {0, -1},
    {0, 1},
    {1, -1},
    {1, 0},
    {1, 1},
};

bool is_emitter(struct CellArg *c_arg) {
    return c_arg->row == 0 && c_arg->col == 0;
}

static const struct timespec FRAME_INTERVAL = {
    .tv_sec = 0,
    .tv_nsec = 500000000,
};

// SAFETY: assumes exclusive access to `board`
// also we require that this isn't parallel because we're printing
void emit(struct CellArg *c_arg, int generation) {
    clear_screen();

    printf("=== GENERATION %d ===\n", generation);

    struct Board *board = c_arg->board;

    for (int row = 0; row < board->dim.rows; row += 1) {
        for (int col = 0; col < board->dim.cols; col += 1) {
            int idx = to_idx(&board->dim, row, col);
            char glyph = board->curr[idx] ? '#' : '.';
            printf("%c", glyph);
        }

        printf("\n");
    }

    bool *temp = board->curr;
    board->curr = board->next;
    board->next = temp;

    nanosleep(&FRAME_INTERVAL, NULL);
}

int self_idx(struct CellArg *c_arg) {
    return to_idx(&c_arg->board->dim, c_arg->row, c_arg->col);
}

// we use "fixed boundaries", aka the convention
// that cells which are out-of-bounds are dead.
int alive_neighbors(struct CellArg *c_arg) {
    int live_neis = 0;

    struct Board *board = c_arg->board;

    for (int i = 0; i < 8; i += 1) {
        int nei_row = c_arg->row + nei[i][0];
        int nei_col = c_arg->col + nei[i][1];

        if (nei_row < 0
            || nei_row >= board->dim.rows
            || nei_col < 0
            || nei_col >= board->dim.cols) {
            continue;
        }

        int idx = to_idx(&board->dim, nei_row, nei_col);
        if (board->curr[idx]) {
            live_neis += 1;
        }
    }

    return live_neis;
}
