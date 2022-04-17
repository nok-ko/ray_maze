// Ray_Maze - Nokko's little Maze Generator, with a shiny new C front-end.

#include <stdlib.h>
#include <raylib.h>
#include <string.h>

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"                     // Required for: IMGUI controls

#undef RAYGUI_IMPLEMENTATION            // Avoid including raygui implementation again

#include "style_cyber.h"

#include "maze.h"
#include "queue.h"
#include "stack.h"
#include "export.h"
#include "import.h"

#define VERSION "0.1.3"

// TODO: Structs and loops instead of lots of function calls?
void draw_row(maze_t *m, size_t offset, int x, int y, int cellWidth, Color *color_data) {
    int *data = m->data + offset;
//    Color *color_p = (color_data + offset);
    const int THIRD = cellWidth / 3;
    // Go corner-by-corner:
    for (int col = 0; col < m->width; col++) {
        // Draw center
        if (*data != NONE)
            DrawRectangle(x + THIRD, y + THIRD, THIRD, THIRD, WHITE);

//        // Always draw corners:
//        DrawRectangle(x, y, THIRD, THIRD, *color_p);
//        DrawRectangle(x+(2 * THIRD), y, THIRD, THIRD, *color_p);
//        DrawRectangle(x, y + (2 * THIRD), THIRD, THIRD, *color_p);
//        DrawRectangle(x+(2 * THIRD), y + (2 * THIRD), THIRD, THIRD, *color_p);

        // Sometimes do the openings:
        if ((*data & N)) DrawRectangle(x + THIRD, y, THIRD, THIRD, WHITE);
        if ((*data & S)) DrawRectangle(x + THIRD, y + (2 * THIRD), THIRD, THIRD, WHITE);
        if ((*data & W)) DrawRectangle(x, y + THIRD, THIRD, THIRD, WHITE);
        if ((*data & E)) DrawRectangle(x + (2 * THIRD), y + THIRD, THIRD, THIRD, WHITE);

        if (*data == NONE) {
//            DrawRectangle(x + THIRD, y + THIRD, THIRD, THIRD, BLACK);
        }

        x += cellWidth;
        data++;
//        color_p++;
    }
}

void draw_row_walls(maze_t *m, size_t offset, int x, int y, int cellWidth, Color color, Color *color_data) {
    int *data = m->data + offset;
    Color *color_p = (color_data + offset);
    const int THIRD = cellWidth / 3;
    // Go corner-by-corner:
    for (int col = 0; col < m->width; col++) {

        if (*data == NONE) {
            DrawRectangle(x + THIRD, y + THIRD, THIRD, THIRD, *color_p);
            goto loop_end;
        }
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

        loop_end:
        x += cellWidth;
        data++;
        color_p++;
    }
}

void draw_maze(maze_t *m, int x, int y, int cwidth, Color *color_data) {
    int width = m->width;
    for (int row = 0; row < m->height; row++) {
        //draw_row(m, width * row, x,y+(cwidth*row),cwidth, BLACK, color_data);
        draw_row(m, width * row, x,y+(cwidth*row),cwidth, color_data);
    }
}

void color_step(maze_t *m, Color *color_data, arr_stack_t *stack, const unsigned int frames, Color from, Color to) {
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
void change_color(Color *color, unsigned int frames, enum ColorAlgorithm algo, struct maze *m, Color gradientStart, Color gradientEnd) {
    if (algo == GRADIENT) {
        float factor = ((float) (frames % m->height * m->width)) / (float)(m->height * m->width);
        *color = ColorAlphaBlend(gradientStart,gradientEnd, ColorAlpha(WHITE, factor));
    } else if (algo == VISITED) {
//       float *hue = ((hue_data + (last.oy * m->width)) + last.ox);
//       *hue = *hue + 60;
//       *color = ColorFromHSV(*hue, 1.0F, 0.7F);
    } else if (algo == JUST_BLACK) {
        *color = BLACK;
    }
}

void draw_colours(const maze_t *m, const int x, const int y, const int cwidth, Color *color_p) {
    for (int oy = 0; oy < m->height; oy++) {
        for (int ox = 0; ox < m->width; ox++) {
            DrawRectangle(ox*cwidth, oy*cwidth, cwidth, cwidth,
                          *((color_p + oy *m->width) + ox));
        }
    }

}

int main(void) {
    const int screen_height = 800;
    const int screen_width = 900;

    InitWindow(screen_height, screen_width, "RayMaze " VERSION);
    GuiLoadStyleCyber();
    SetTargetFPS(60);

    // Generate maze;
    const int sideLength = 81;
    int *box_data = (int *) calloc(sideLength*sideLength, sizeof(int));
    maze_t m = {box_data, sideLength, sideLength};
    fill_maze(&m);
    carve_maze(&m);

    Color *color_data = calloc(sideLength*sideLength, sizeof(Color));

    unsigned int steps = 0;
    size_t initial_cap = (m.width * m.height) / 4;
    arr_stack_t *stack = new_stack(sizeof(pos_t), initial_cap);
    pos_t current = {0, 0};

    const int offset_x = 0;
    const int offset_y = 0;
    // cell size, cwidth^2 pixels
    const int cwidth = 6;
//    float *hue_data = (float *) malloc(sideLength * sideLength * sizeof(float));
    const float pattern = 60.0F;

    // HACK: This only works on Mac (maybe BSD?), whoops.
//    memset_pattern4(hue_data,  &pattern, sideLength * sideLength * sizeof(float));

    struct history_el {int ox; int oy; bool shouldCarve;};

    queue history_queue;
    createQueue(&history_queue, 50, sizeof(struct history_el));
    fill_maze(&m);
    stack->len = 0;
    push(stack, &current);

    // UI State
    bool shouldGenerate = false;
    float slider = 1.0f;
    Color gradientStartColor = BLUE;
    Color gradientEndColor = GREEN;
    unsigned int hoverTime = 0;
    bool showColours = true;
    bool showMaze = true;

    char *filename_buf = malloc(sizeof(char) * 50);
    strcpy(filename_buf, "realmaze.maz");
    bool editing_filename = false;

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);
        DrawText("RayMaze!", screen_width - 30, 30, 20, LIGHTGRAY);

        if (showColours)
            draw_colours(&m, offset_x, offset_y, cwidth, (Color *) color_data);

        if (showMaze)
            draw_maze(&m,offset_x, offset_y, cwidth, (Color *) color_data);

        GuiEnable();
        const float BASELINE = 700.0f;
        const float LEFT_MARGIN = 48.0f;
        float x = LEFT_MARGIN;
        if(GuiButton((Rectangle){x, BASELINE, 80, 32}, "Generate!")) {
            shouldGenerate = true;
            fill_maze(&m);
            memset(color_data, 0, m.width * m.height * sizeof(Color));
            stack->len = 1;
        }

        if(GuiButton((Rectangle){x += 16 + 80, BASELINE, 80, 32}, "Reset!")) {
            fill_maze(&m);
            memset(color_data, 0, m.width * m.height * sizeof(Color));
            stack->len = 1;
            peek(stack, &current);
            stack->len = 0;
            steps = 0;
        }

        slider = GuiSliderBar(
                (Rectangle){(x += 80 + 16), BASELINE, 192, 32},
                "-", "+",
                slider, 0.0f, 60.0f);

        Rectangle button_bounds = {(x += 192 + 16), BASELINE, 32, 32};
        if(GuiButton(button_bounds, "R")) {
            slider = 1.0f;
        }

        Vector2 mousePos = GetMousePosition();
        // Tooltip
        if(CheckCollisionPointRec(mousePos, button_bounds)) {
            if (hoverTime > 20) {
                Color strokeColor = GetColor(GuiGetStyle(BUTTON, BORDER_COLOR_NORMAL));
                Color fillColor = GetColor(GuiGetStyle(BUTTON, BASE_COLOR_NORMAL));
                Color textColor = GetColor(GuiGetStyle(BUTTON, TEXT_COLOR_NORMAL));
                int borderWidth = GuiGetStyle(BUTTON, BORDER_WIDTH);
                Rectangle tooltipBounds = (Rectangle) {mousePos.x, mousePos.y - 16.0f, 96, 32};
                GuiDrawRectangle(tooltipBounds, borderWidth, strokeColor, fillColor);
                GuiDrawText("Reset Speed", tooltipBounds, GUI_TEXT_ALIGN_CENTER,textColor);
            } else {
                hoverTime++;
            }
        } else {
            hoverTime = 0;
        }

        // Next row
        x = LEFT_MARGIN;
        float y = BASELINE - 40.0f;

            // “Show Colours” Checkbox:
        showColours = GuiCheckBox((Rectangle) {x, y, 32, 32}, "Show Colours?", showColours);
        if (showColours)
            GuiDrawIcon(RAYGUI_ICON_EYE_ON, x, y, 2,
                        GetColor(GuiGetStyle(BUTTON, TEXT_COLOR_DISABLED)));
        else
            GuiDrawIcon(RAYGUI_ICON_EYE_ON, x, y, 2,
                        GetColor(GuiGetStyle(BUTTON, TEXT_COLOR_NORMAL)));

        // “Show Maze” Checkbox:
        showMaze = GuiCheckBox((Rectangle) {x += (176 + 16), y, 32, 32}, "Show Maze?", showMaze);
        if (showMaze)
            GuiDrawIcon(RAYGUI_ICON_EYE_ON, x, y, 2,
                    GetColor(GuiGetStyle(BUTTON, TEXT_COLOR_DISABLED)));
        else
            GuiDrawIcon(RAYGUI_ICON_EYE_ON, x, y, 2,
                    GetColor(GuiGetStyle(BUTTON, TEXT_COLOR_NORMAL)));

        // Gradient Colour Pickers:
        if (shouldGenerate) GuiDisable();
        gradientStartColor = GuiColorPicker((Rectangle){(x += 32), y - 140, 64, 64},
                                            "Gradient Start", gradientStartColor);
        gradientEndColor = GuiColorPicker((Rectangle){(x += 96), y - 140, 64, 64},
                                          "Gradient End", gradientEndColor);
        if (shouldGenerate) GuiEnable();

        // Next row
        x = LEFT_MARGIN;
        y -= 40.0f;

//        GuiDisable();
        if(GuiButton((Rectangle){x, y, 160, 32}, "Dump unpacked")) {
            write_maz(&m, PM_Unpacked, "unpacked.maz");
        }
//        GuiEnable();

        if(GuiButton((Rectangle){x += 192, y, 160, 32}, "Dump to \".MAZ\" file")) {
            write_maz(&m, PM_Packed, "realmaze.maz");
        }

        Rectangle textbox_bounds = (Rectangle){x += 192, y, 160, 24};

        bool lmb_just_released = IsMouseButtonReleased(MOUSE_BUTTON_LEFT);
        if (lmb_just_released && CheckCollisionPointRec(mousePos, textbox_bounds)) {
            editing_filename = true;
        } else if (lmb_just_released) {
            editing_filename = false;
        }

        GuiTextBox(textbox_bounds, filename_buf, 50,editing_filename);

        if (GuiButton((Rectangle) {x, y += 40, 160, 32}, "Read .MAZ File")) {
            struct maze *mptr = read_maz(filename_buf);
            int *old_data = m.data;
            m.data = mptr->data;
            free(old_data);
            free(mptr);
            GuiDisable();
        }





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
                change_color(color_data + (el->oy * m.width) + el->ox, steps, GRADIENT, &m, gradientStartColor, gradientEndColor);
                change_color(color_data + ((el->oy+n.oy) * m.width) + el->ox+n.ox, steps, GRADIENT, &m, gradientStartColor, gradientEndColor);
                el->shouldCarve = false;
            }
        }

        if (shouldGenerate) {
            if (!stack->len) {
                emptyQueue(&history_queue);
                steps = 0;
            }
            if (steps == 0) {
                *color_data = gradientStartColor;
            }
            bool last_carved_new = false;
            bool carved_new = true;
            for (int i = 0; i < (int) (slider) && stack->len; i++) {
                steps++;
                pos_t last = {0, 0};
                // This loop is here to skip backtracking steps, and do some other stuff (colours) during backtracking.
                for (;stack->len;) {
                    carved_new = carve_step(&m, stack, &current);
                    // We just backtracked!
                    if (!carved_new) {
                        // Colour in while backtracking :)
                        color_step(&m, color_data, stack, steps, gradientStartColor, gradientEndColor);
                        steps++;
                    } else {
                        // Didn't backtrack, so don't skip!
                        color_step(&m, color_data, stack, steps, gradientStartColor, gradientEndColor);
                        steps++;
                        break;
                    }
                }
                // Extra colour step after exiting the loop
                if (carved_new) {
                    push(stack, &last);
//                    color_step(&m, color_data, stack, steps, RED, RED);
                    pop(stack, &last);
                    steps++;
                }
            }
            enqueue(&history_queue, &(struct history_el) {current.ox, current.oy, true});
            if (!stack->len) {
                shouldGenerate = false;
            }

        }
        // Clear maze, clear generation stack.
        if (IsMouseButtonReleased(MOUSE_BUTTON_RIGHT)) {
            fill_maze(&m);
            stack->len = 1;
            peek(stack, &current);
        }
        steps++;
        EndDrawing();
    }

    CloseWindow();
    free(box_data);
    free_stack(stack);
    free(color_data);
    free(history_queue.values);
    free(filename_buf);
    return 0;
}
