/**
An implementation of the recursive backtracker maze
generation algorithm in C. Implemented recursively.

Author: nokko
Date: March 2nd
Look on my works, ye mighty, and cringe at my horrible code.

Credit to Jamis Buck for teaching me this wonderful algorithm.
(https://weblog.jamisbuck.org/2010/12/27/maze-generation-recursive-backtracking)
**/

#include <stdlib.h>
#include <stdbool.h>
#include "maze.h"

// Fill a maze with walls, i.e. NONE values
void fill_maze(struct maze *m) {
    for (int row = 0; row < m->height; row++) {
        for (int col = 0; col < m->width; col++) {
            m->data[row * m->width + col] = NONE;
        }
    }
}

// get a random neighbour!
struct neighbour get_neighbour(int ox, int oy, struct maze *m) {
    // check out-of-bounds
    bool can_N, can_E, can_S, can_W;
    can_N = (oy - 1) >= 0;
    can_E = (ox + 1) < m->width;
    can_S = (oy + 1) < m->height;
    can_W = (ox - 1) >= 0;

    struct neighbour neighbours[4];
    int count = 0;
    // North: (0, -1)
    if (can_N) {
        int *data = &m->data[(m->width * (oy - 1)) + (ox)];
        if (*data == NONE) {
            neighbours[count] = (struct neighbour) {0, -1, N, data};
            count++;
        }
    }
    // East: (+1, 0)
    if (can_E) {
        int *data = &m->data[(m->width * (oy)) + (ox + 1)];
        if (*data == NONE) {
            neighbours[count] = (struct neighbour) {1, 0, E, data};
            count++;
        }
    }
    // South: (0, +1)
    if (can_S) {
        int *data = &m->data[(m->width * (oy + 1)) + (ox)];
        if (*data == NONE) {
            neighbours[count] = (struct neighbour) {0, 1, S, data};
            count++;
        }
    }
    // West: (-1, 0)
    if (can_W) {
        int *data = &m->data[(m->width * (oy)) + (ox - 1)];
        if (*data == NONE) {
            neighbours[count] = (struct neighbour) {-1, 0, W, data};
            count++;
        }
    }
    struct neighbour out = {.ox=0, .oy=0};
    // No valid neighbours.
    if (count == 0) {
        // Can't keep going, return!
        //printf("can't get neighbour!\n");
        return out;
    }
    // Very wasteful, we throw away a lot of computation here.
    // Honestly, I've stopped caring.
    // PRO TIP: MAKE SURE YOU DON'T INDEX BY NEGATIVE VALUES.
    // THIS BUG TOOK AWAY VALUABLE HOURS OF MY LIFE.
    int rand = arc4random();
    out = neighbours[rand % count];
    return out;
}

// LUT for opposite directions
static const int opposite[N+1] = {
        0,
        // [1] => W, opposite = E
        E,
        // [2] => S, opposite = N
        N,
        // [4] => E, opposite = W
        0,
        W,
        0,
        0,
        0,
        // [8] => N, opposite = S
        S
};

void carve(int ox, int oy, struct maze *m) {
    int *current = &m->data[(m->width * oy) + ox];
    while (true) {
        // Pick a random neighbour.
        struct neighbour nb = get_neighbour(ox, oy, m);
        // No neighbours? Done!
        if (nb.ox == 0 && nb.oy == 0) {
            return;
        }

        // Check if there's any path to the neighbour
        // If not:
        if (!(*current & nb.direction) && *nb.data == NONE) {
            *nb.data |= opposite[nb.direction]; // carved!
            // carve self!
            *current |= nb.direction;
            // recur!
            carve(ox + nb.ox, oy + nb.oy, m);
        }

    }
}

void carve_maze(maze_t *m) {
    // Fill the maze, we'll be carving paths out.
    fill_maze(m);
    carve(0, 0, m);
}

//int main(int argc, char **argv) {
//    int side_length = 10;
//    if (argc > 1) {
//        // Try to parse length argument
//        int user_length;
//        int converted = sscanf(argv[1], "%d", &user_length);
//        if (converted >= 1) {
////            a->b
//            side_length = user_length;
//        }
//    } else {
//        printf("Defaulting to a side length of 10.\n"
//                "Call with side length argument to generate a maze of custom size.\n"
//                "\t ./c_maze [side_length]\n");
//    }
//
//    int *box_data = (int *) calloc(side_length*side_length, sizeof(int));
//    maze_t box_maze = {box_data, side_length, side_length};
//
//    carve_maze(&box_maze);
//
//    free(box_data);
//    return 0;
//}
