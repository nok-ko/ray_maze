#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>
#include "./maze.c"

#define VERSION "0.0.1"

void draw_row(maze_t *m, size_t offset, int x, int y, int cellWidth, Color color) {
    int *data = m->data + offset;
    const int THIRD = cellWidth / 3;
    // Go corner-by-corner:
    for (int col = 0; col < m->width; col++) {
        // Always draw corners:
        DrawRectangle(x, y, THIRD, THIRD, color);
        DrawRectangle(x+(2 * THIRD), y, THIRD, THIRD, color);
        DrawRectangle(x, y + (2 * THIRD), THIRD, THIRD, color);
        DrawRectangle(x+(2 * THIRD), y + (2 * THIRD), THIRD, THIRD, color);

        // Sometimes do the openings:
        if (!(*data & N)) DrawRectangle(x + THIRD, y, THIRD, THIRD, color); else {}
        if (!(*data & S)) DrawRectangle(x + THIRD, y + (2 * THIRD), THIRD, THIRD, color); else {}
        if (!(*data & W)) DrawRectangle(x, y + THIRD, THIRD, THIRD, color); else {}
        if (!(*data & E)) DrawRectangle(x + (2 * THIRD), y + THIRD, THIRD, THIRD, color); else {}

        x += cellWidth;
        data++;
    }
}

void draw_maze(maze_t *m, int x, int y, int cwidth) {
    int width = m->width;
    for (int row = 0; row < m->height; row++) {
        draw_row(m, width * row, x,y+(cwidth*row),cwidth, BLACK);
    }
}

int main(void) {
    const int screen_height = 800;
    const int screen_width = 450;

    InitWindow(screen_height, screen_width, "RayMaze " VERSION);
    SetTargetFPS(60);

    // Generate maze;
    const int sideLength = 30;
    int *box_data = (int *) calloc(sideLength*sideLength, sizeof(int));
    maze_t m = {box_data, sideLength, sideLength};
    fill_maze(&m);
    carve_maze(&m);

    unsigned int frames = 0;
    size_t initial_cap = (m.width * m.height) / 4;
    arr_stack_t *stack = new_stack(sizeof(pos_t), initial_cap);
    pos_t current = {0, 0};
    while (!WindowShouldClose()) {
        BeginDrawing();
        {
            if (frames % 2 == 0 && stack->len > 0) {
                carve_step(&m, stack, &current);
            }
            ClearBackground(RAYWHITE);
            DrawText("RayMaze!", screen_width - 30, 30, 20, LIGHTGRAY);
            draw_maze(&m, 10, 10, 9);
            DrawRectangle(10 + (9 * current.ox) + 3, 10 + (9 * current.oy) + 3, 3, 3, MAGENTA);
            if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                current = (pos_t) {0, 0};
                fill_maze(&m);
                push(stack, &current);
            }
            frames++;
        }
        EndDrawing();
    }

    CloseWindow();
    free(box_data);
    free_stack(stack);
    return 0;
}
