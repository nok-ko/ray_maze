//
// Created by Nokko on 2022-04-06.
//
#include <locale.h>
#include <stdbool.h>
#include <wchar.h>
#include "maze.h"

#ifndef RAY_MAZE_ASCII_MAZE_DISPLAY_H
#define RAY_MAZE_ASCII_MAZE_DISPLAY_H

// Characters to use when drawing
#define FILLCHAR ((wint_t)9619)
#define NONECHAR ((wint_t)128556)


// Maybe should be called “FillUnless…”
// Sets the char at `location` to NONECHAR if `condition` is met.
void fillIf(wint_t *location, bool condition) {
    if (condition) {
        *location = NONECHAR;
    } else {
        *location = FILLCHAR;
    }
};

void display_row(const int *data, const struct maze *m) {
    const int width = m->width;
    // Init strings
    wint_t *top    = (wint_t *) calloc(width * 3 + 1, sizeof(wint_t));
    wint_t *middle = (wint_t *) calloc(width * 3 + 1, sizeof(wint_t));
    wint_t *bottom = (wint_t *) calloc(width * 3 + 1, sizeof(wint_t));

    if (top == NULL || middle == NULL || bottom == NULL) {
        printf("failed to allocate memory! exiting.\n");
        exit(1);
    }

    // Fill strings with data: Top pass
    for (int i = 0; i < width; i++) {
        int base = i * 3; // char array is 3x the size of the data

        // Corners don't change.
        top[base] = FILLCHAR;
        top[base + 2] = FILLCHAR;
        // Check only the North bit
        fillIf(&top[base + 1], data[i] & N);

        // Middle pass: check two bits
        // Centre doesn't change
        middle[base + 1] = NONECHAR;
        fillIf(&middle[base], data[i] & W);
        fillIf(&middle[base + 2], data[i] & E);

        // Bottom pass
        bottom[base] = FILLCHAR;
        bottom[base + 2] = FILLCHAR;
        fillIf(&bottom[base + 1], data[i] & S);
    }

    // Use the strings!
    printf("%ls\n", top);
    printf("%ls\n", middle);
    printf("%ls\n", bottom);

    // Always free dynamically allocated memory.
    free(top);
    free(middle);
    free(bottom);
}

void display_maze(struct maze m) {
    int size = m.width * m.height;
    for (int row = 0; row < m.height; row++) {
        display_row(&m.data[row * m.width], &m);
    }
}

#endif //RAY_MAZE_ASCII_MAZE_DISPLAY_H
