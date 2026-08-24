
the game of life:

https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life

develop the game of life using two grids or boards ('current' and 'next').
have a separate thread per cell. each thread gets coordinates (x, y),
and in a loop, will calculate the next-generation value for its cell,
based on its neighbours - as defined in the rules section in the above
wikipedia article.

use a barrier to coordinate the transition between generations.

one of the threads (the one handling coordinates 0,0) will emit
the contents of the 'current' generation (including a title with the
current generation number) on screen, and will switch between the
'curr' and 'next' grids.

hint: you'll need to use the barrier more then once per loop.

note 1: use a hard-coded setup as the 'seed'. some examples:
        // Seed an initial glider pattern
        // use a 12x12 grid and 15 generations
        curr[1][2] = curr[2][3] = curr[3][1] = curr[3][2] = curr[3][3] = 1;

        // seed 10 consecutive horizontal cells in the middle row (row 8)
        // use a 17x17 grid and 20 generations
        for (int j = 3; j < 13; j++) curr[8][j] = 1;
note 2: have a 1-second sleep after drawing the board before continuing,
        so you'll be able to see what is going on.

note 3: use the following to clear the screen right before emitting
        the current board - it'll make the output look like an animation:

        printf("\033[H\033[J");
