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
#include "stack.h"

// Fill a maze with walls, i.e. NONE values
void fill_maze(struct maze *m) {
    for (int row = 0; row < m->height; row++) {
        for (int col = 0; col < m->width; col++) {
            m->data[row * m->width + col] = NONE;
        }
    }
}

// get a random neighbour!
neighbour_t get_neighbour(int ox, int oy, struct maze *m) {
    typedef struct {
        int direction;
        char offset_x;
        char offset_y;
        bool isNotOOB;
    } direction_t;

    direction_t directions[4] = {
            N,  0, -1, (oy - 1) >= 0,
            E,  1, 0, (ox + 1) < m->width,
            S,  0, 1, (oy + 1) < m->height,
            W,  -1, 0, (ox - 1) >= 0,
    };

    neighbour_t neighbours[4];
    int count = 0;
    for (int i = 0; i < 4; i++) {
        const direction_t current = directions[i];
        if (current.isNotOOB) {
            int *data = &m->data[(m->width * (oy + current.offset_y)) + (ox + current.offset_x)];
            if (*data == NONE) {
                neighbours[count] = (neighbour_t) {
                    current.offset_x,
                    current.offset_y,
                    current.direction,
                    data
                };
                count++;
            }
        }
    }
    neighbour_t out = {.ox=0, .oy=0};
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
    out = neighbours[arc4random() % count];
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

typedef struct {int ox; int oy;} pos_t;

void carve_step(struct maze *m, arr_stack_t *stack, pos_t *current);

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


void carve_iter(int ox, int oy, struct maze *m) {
    size_t initial_cap = (m->width * m->height) / 4;
    arr_stack_t *stack = new_stack(sizeof(pos_t), initial_cap);
    pos_t current = {ox, oy};
    push(stack, &current);
    while (stack->len) {
        carve_step(m, stack, &current);
    }
    free_stack(stack);
}

void carve_step(struct maze *m, arr_stack_t *stack, pos_t *current) {
    int *cell = &m->data[(m->width * (*current).oy) + (*current).ox];
    neighbour_t nb = get_neighbour((*current).ox, (*current).oy, m);
    // no neighbours here, move on
    if (nb.ox != 0 || nb.oy != 0) {
        // Check if there's any path to the neighbour
        // If not:
        if (!(*cell & nb.direction) && *nb.data == NONE) {
            *nb.data |= opposite[nb.direction]; // carved!
            // carve self!
            *cell |= nb.direction;
            // recur!
            push(stack, &((pos_t) {current->ox + nb.ox, current->oy + nb.oy}));
            peek(stack, current);
        }
    } else {
        pop(stack, current);
    }
}
void carve_step_fishbone(struct maze *m, arr_stack_t *stack, pos_t *current) {
    int *cell = &m->data[(m->width * (*current).oy) + (*current).ox];
    neighbour_t nb = get_neighbour((*current).ox, (*current).oy, m);
    // no neighbours here, move on
    if (nb.ox != 0 || nb.oy != 0) {
        // Check if there's any path to the neighbour
        // If not:
        if (!(*cell & nb.direction) && *nb.data == NONE) {
            *nb.data |= opposite[nb.direction]; // carved!
            // carve self!
            *cell |= nb.direction;
            // recur!
            push(stack, &((pos_t) {current->ox + nb.ox, current->oy + nb.oy}));
//            peek(stack, current);
        }
    } else {
        pop(stack, current);
    }
}

void carve_maze(maze_t *m) {
    // Fill the maze, we'll be carving paths out.
    fill_maze(m);
    carve_iter(0, 0, m);
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
