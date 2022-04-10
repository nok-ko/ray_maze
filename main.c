#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>
#include "./maze.c"

#define VERSION "0.0.1"

void draw_row(maze_t *m, size_t offset, int x, int y, int cellWidth, Color color, Color *color_data) {
    int *data = m->data + offset;
    Color *color_p = (color_data + offset);
    const int THIRD = cellWidth / 3;
    // Go corner-by-corner:
    for (int col = 0; col < m->width; col++) {
        // Always draw corners:
        DrawRectangle(x, y, THIRD, THIRD, *color_p);
        DrawRectangle(x+(2 * THIRD), y, THIRD, THIRD, *color_p);
        DrawRectangle(x, y + (2 * THIRD), THIRD, THIRD, *color_p);
        DrawRectangle(x+(2 * THIRD), y + (2 * THIRD), THIRD, THIRD, *color_p);

        // Sometimes do the openings:
        if (!(*data & N)) DrawRectangle(x + THIRD, y, THIRD, THIRD, *color_p); else {}
        if (!(*data & S)) DrawRectangle(x + THIRD, y + (2 * THIRD), THIRD, THIRD, *color_p); else {}
        if (!(*data & W)) DrawRectangle(x, y + THIRD, THIRD, THIRD, *color_p); else {}
        if (!(*data & E)) DrawRectangle(x + (2 * THIRD), y + THIRD, THIRD, THIRD, *color_p); else {}

        if (*data == NONE) {
//            DrawRectangle(x + THIRD, y + THIRD, THIRD, THIRD, BLACK);
        }

        x += cellWidth;
        data++;
        color_p++;
    }
}

void draw_maze(maze_t *m, int x, int y, int cwidth, Color *color_data) {
    int width = m->width;
    for (int row = 0; row < m->height; row++) {
        draw_row(m, width * row, x,y+(cwidth*row),cwidth, BLACK, color_data);
    }
}

bool colors_not_equal(Color first, Color second) {
    bool eq = 1;
    eq = eq && (first.r != second.r);
    return 0;
}

void color_step(maze_t *m, Color *color_data, arr_stack_t *stack, const unsigned int frames) {
    pos_t last;
    const int size = m->width * m->height;
    if (peek(stack, &last) != 0) {
        Color *current = ((color_data + (last.oy * m->width)) + last.ox);
        Vector3 hsv = ColorToHSV(RED);
        float factor = ((float) (frames % size)) / (float)(size);
        Color trans;
        if (factor < 0.33) {
            trans = ColorAlphaBlend(PINK, WHITE, ColorAlpha(WHITE, factor));
        } else if (factor < 0.66){
            trans = ColorAlphaBlend(RAYWHITE, BLUE, ColorAlpha(WHITE, factor * (float)0.2));
        } else {
            trans = ColorAlphaBlend(WHITE, BLUE, ColorAlpha(WHITE, factor));
        }
        Color result = ColorFromHSV(hsv.x, hsv.y, hsv.z);
        *current = trans;
    }
}

int main(void) {
    const int screen_height = 800;
    const int screen_width = 900;

    InitWindow(screen_height, screen_width, "RayMaze " VERSION);
    SetTargetFPS(60);

    // Generate maze;
    const int sideLength = 80;
    int *box_data = (int *) calloc(sideLength*sideLength, sizeof(int));
    maze_t m = {box_data, sideLength, sideLength};
    fill_maze(&m);
    carve_maze(&m);

    Color *color_data = calloc(sideLength*sideLength, sizeof(Color));

    unsigned int frames = 0;
    size_t initial_cap = (m.width * m.height) / 4;
    arr_stack_t *stack = new_stack(sizeof(pos_t), initial_cap);
    pos_t current = {0, 0};

    const int offset_x = 2;
    const int offset_y = 2;
    // cell size, cwidth^2 pixels
    const int cwidth = 9;
    while (!WindowShouldClose()) {
        BeginDrawing();
        {
            if (frames % 1 == 0 && stack->len > 0) {
                for (int i = 0; i < 60 && stack->len > 0; i++) {
                    carve_step(&m, stack, &current);
                    color_step(&m, color_data, stack, frames);
                    frames++;
                }
            }
            ClearBackground(WHITE);
            DrawText("RayMaze!", screen_width - 30, 30, 20, LIGHTGRAY);
            draw_maze(&m,offset_x, offset_y, cwidth, (Color *) color_data);
            DrawRectangle(offset_x + (cwidth * current.ox) + (cwidth/3),
                          offset_y + (cwidth * current.oy) + (cwidth/3),
                          cwidth/3, cwidth/3,
                          MAGENTA);
            if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                frames = 0;
                *color_data = RED;
                current = (pos_t) {0, 0};
                fill_maze(&m);
                push(stack, &current);
            }
        }
        EndDrawing();
    }

    CloseWindow();
    free(box_data);
    free_stack(stack);
    return 0;
}
