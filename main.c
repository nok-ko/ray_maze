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
    maze_t box_maze = {box_data,sideLength,sideLength};
    carve_maze(&box_maze);

    while (!WindowShouldClose()) {
        BeginDrawing();
        {
            ClearBackground(RAYWHITE);
            DrawText("RayMaze!", screen_width - 30, 30, 20, LIGHTGRAY);
            draw_maze(&box_maze, 10, 10, 9);
            if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                carve_maze(&box_maze);
            }
        }
        EndDrawing();
    }

    CloseWindow();
    free(box_data);
    return 0;
}
