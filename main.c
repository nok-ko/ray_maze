// Ray_Maze - Nokko's little Maze Generator, with a shiny new C front-end.

#include <stdlib.h>
#include <raylib.h>
#include <string.h>
#include "maze.h"
#include "queue.h"
#include "stack.h"

#define VERSION "0.0.3"

// TODO: Structs and loops instead of lots of function calls?
void draw_row(maze_t *m, size_t offset, int x, int y, int cellWidth, Color color, Color *color_data) {
    int *data = m->data + offset;
    Color *color_p = (color_data + offset);
    const int THIRD = cellWidth / 3;
    // Go corner-by-corner:
    for (int col = 0; col < m->width; col++) {
        // Draw background
        if (*data != NONE)
            DrawRectangle(x, y, cellWidth, cellWidth, WHITE);

        // Always draw corners:
        DrawRectangle(x, y, THIRD, THIRD, *color_p);
        DrawRectangle(x+(2 * THIRD), y, THIRD, THIRD, *color_p);
        DrawRectangle(x, y + (2 * THIRD), THIRD, THIRD, *color_p);
        DrawRectangle(x+(2 * THIRD), y + (2 * THIRD), THIRD, THIRD, *color_p);

        // Sometimes do the openings:
        if (!(*data & N)) DrawRectangle(x + THIRD, y, THIRD, THIRD, *color_p);
        if (!(*data & S)) DrawRectangle(x + THIRD, y + (2 * THIRD), THIRD, THIRD, *color_p);
        if (!(*data & W)) DrawRectangle(x, y + THIRD, THIRD, THIRD, *color_p);
        if (!(*data & E)) DrawRectangle(x + (2 * THIRD), y + THIRD, THIRD, THIRD, *color_p);

        if (*data == NONE) {
            DrawRectangle(x + THIRD, y + THIRD, THIRD, THIRD, *color_p);
        }

        x += cellWidth;
        data++;
        color_p++;
    }
}

void draw_row_walls(maze_t *m, size_t offset, int x, int y, int cellWidth, Color color, Color *color_data) {
    int *data = m->data + offset;
    Color *color_p = (color_data + offset);
    const int THIRD = cellWidth / 3;
    // Go corner-by-corner:
    for (int col = 0; col < m->width; col++) {
        // Sometimes draw corners:
//        if (*data & N & W)
//            DrawRectangle(x, y, THIRD, THIRD, *color_p);
//        if (*data & N & E)
//            DrawRectangle(x+(2 * THIRD), y, THIRD, THIRD, *color_p);
//        if (*data & S & W)
//            DrawRectangle(x, y + (2 * THIRD), THIRD, THIRD, *color_p);
//        if (*data & S & E)
//            DrawRectangle(x+(2 * THIRD), y + (2 * THIRD), THIRD, THIRD, *color_p);

//        color_p = &(BLUE);
        // Sometimes draw walls:
        if (!(*data & N)) DrawRectangle(x, y, cellWidth, THIRD, *color_p);
        if (!(*data & S)) DrawRectangle(x, y + (2 * THIRD), cellWidth, THIRD, *color_p);
        if (!(*data & W)) DrawRectangle(x, y, THIRD, cellWidth, *color_p);
        if (!(*data & E)) DrawRectangle(x + (2 * THIRD), y, THIRD, cellWidth, *color_p);

        if (*data == NONE) {
            DrawRectangle(x + THIRD, y + THIRD, THIRD, THIRD, *color_p);
        }

        x += cellWidth;
        data++;
        color_p++;
    }
}

void draw_maze(maze_t *m, int x, int y, int cwidth, Color *color_data) {
    int width = m->width;
    for (int row = 0; row < m->height; row++) {
        //draw_row(m, width * row, x,y+(cwidth*row),cwidth, BLACK, color_data);
        draw_row(m, width * row, x,y+(cwidth*row),cwidth, BLACK, color_data);
    }
}

void color_step(maze_t *m, Color *color_data, arr_stack_t *stack, const unsigned int frames, float *hue_data, Color from, Color to) {
    pos_t last;
    const int size = m->width * m->height;
    if (peek(stack, &last) != 0) {
        Color *current = ((color_data + (last.oy * m->width)) + last.ox);
        float factor = ((float) (frames % size)) / (float)(size);
        Color trans;
//        if (factor < 0.33) {
//            trans = ColorAlphaBlend(PINK, WHITE, ColorAlpha(WHITE, factor));
//        } else if (factor < 0.66){
//            trans = ColorAlphaBlend(RAYWHITE, BLUE, ColorAlpha(WHITE, factor * (float)0.2));
//        } else {
//            trans = ColorAlphaBlend(WHITE, BLUE, ColorAlpha(WHITE, factor));
//        }
        trans = ColorAlphaBlend(from, to, ColorAlpha(WHITE, factor));
        *current = trans;
    }
}

enum ColorAlgorithm {
    GRADIENT,
    VISITED,
    JUST_BLACK
};

// Change the given colour based on whichever algo you like
void change_color(Color *color, unsigned int frames, enum ColorAlgorithm algo, struct maze *m) {
    if (algo == GRADIENT) {
        float factor = ((float) (frames % m->height * m->width)) / (float)(m->height * m->width);
        *color = ColorAlphaBlend(ORANGE, MAGENTA, ColorAlpha(WHITE, factor));
    } else if (algo == VISITED) {
//       float *hue = ((hue_data + (last.oy * m->width)) + last.ox);
//       *hue = *hue + 60;
//       *color = ColorFromHSV(*hue, 1.0F, 0.7F);
    } else if (algo == JUST_BLACK) {
        *color = BLACK;
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
    float *hue_data = (float *) malloc(sideLength * sideLength * sizeof(float));
    const float pattern = 60.0F;
    memset_pattern4(hue_data,  &pattern, sideLength * sideLength * sizeof(float));


    struct history_el {int ox; int oy; bool shouldCarve};

    queue history_queue;
    createQueue(&history_queue, 50, sizeof(struct history_el));
    fill_maze(&m);
    stack->len = 0;
    push(stack, &current);
    while (!WindowShouldClose()) {
        BeginDrawing();
        // Generate passively…
//        if (false && frames % 2 == 0 && stack->len > 0) {
//            for (int i = 0; i < 1 && stack->len > 0; i++) {
//                frames++;
//                while (stack->len && !carve_step(&m, stack, &current)) {
//                    enqueue(&history_queue, &current);
//                }
//                enqueue(&history_queue, &current);
//                color_step(&m, color_data, stack, frames);
//            }
//        }
        ClearBackground(BLACK);
        DrawText("RayMaze!", screen_width - 30, 30, 20, LIGHTGRAY);
        draw_maze(&m,offset_x, offset_y, cwidth, (Color *) color_data);
//            DrawRectangle(offset_x + (cwidth * current.ox) + (cwidth/3),
//                          offset_y + (cwidth * current.oy) + (cwidth/3),
//                          cwidth/3, cwidth/3,
//                          MAGENTA);

        // Carve through history…
        for (size_t i = 0; i < history_queue.size && isFull(&history_queue); i++) {
            break;
            struct history_el *el = (history_queue.values + i*(sizeof(struct history_el)));
            DrawRectangle(offset_x + (cwidth * el->ox) + (cwidth/3),
                          offset_y + (cwidth * el->oy) + (cwidth/3),
                          cwidth/3, cwidth/3,
                          BLUE);
            struct neighbour n = get_neighbour(el->ox, el->oy, &m);
            if ((arc4random() % 100 > 50) && n.direction != NONE && isFull(&history_queue) && el->shouldCarve) {
                carve_from(&m, &(pos_t) {el->ox, el->oy}, &n);
                push(stack, &((pos_t) {el->ox+n.ox, el->oy+n.oy}));
                change_color(color_data + (el->oy * m.width) + el->ox, frames, GRADIENT, &m);
                change_color(color_data + ((el->oy+n.oy) * m.width) + el->ox+n.ox, frames, GRADIENT, &m);
                el->shouldCarve = false;
            }
        }

        // Generate while LMB is held.
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            if (!stack->len) {
                memset_pattern4(hue_data, &pattern, sideLength * sideLength * sizeof(float));
                emptyQueue(&history_queue);
                frames = 0;
            }
            *color_data = MAGENTA;
            for (int i = 0; i < 3 && stack->len; i++) {
                frames++;
                while (stack->len && !carve_step(&m, stack, &current)){
                    enqueue(&history_queue, &(struct history_el) {current.ox, current.oy, true});
                    color_step(&m, color_data, stack, frames, hue_data,ORANGE,MAGENTA);
//                    change_color((color_data + (m.width * current.oy) + current.ox), frames, VISITED, &m);
                    frames++;
                }
                color_step(&m, color_data, stack, frames, hue_data, BLACK, BLACK);
//                change_color((color_data + (m.width * current.oy) + current.ox), frames, VISITED, &m);
            }
            enqueue(&history_queue, &(struct history_el) {current.ox, current.oy, true});
            color_step(&m, color_data, stack, frames, hue_data, BLACK, BLACK);

        }
        // Clear maze, clear generation stack.
        if (IsMouseButtonReleased(MOUSE_BUTTON_RIGHT)) {
            fill_maze(&m);
            stack->len = 1;
            peek(stack, &current);
//            stack->len = 0;
        }
        frames++;
        EndDrawing();
    }

    CloseWindow();
    free(box_data);
    free_stack(stack);
    free(color_data);
    free(history_queue.values);
    return 0;
}
