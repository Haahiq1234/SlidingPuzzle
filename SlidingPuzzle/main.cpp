#include "raylib.h"
#include <string.h>
#include <iostream>

using namespace std;



void MouseButtonPress(); // Ill write what to do in comments and who will implement this function
int pos_to_index(int i, int j);
void index_to_pos(int index, int &i, int &j);
bool is_valid_move(int i, int j, int free_i, int free_j, int w, int h);
void mouse_on_grid_pos(int &i, int &j, float cw, float ch);
void swap_position(int grid[], int i, int j);
//using namespace std;

void DrawCenteredText(const char* cstr, int x, int y, int font_size, Color col) {
    DrawText(cstr, x - MeasureText(cstr, font_size) / 2, y - font_size / 2, font_size, col);
}

int main(void)
{
    int window_width = 800, window_height = 800;
    InitWindow(window_width, window_height, "raylib example - basic window");

    int width = 3, height = 3;
    int blank_i = width - 1, blank_j = height - 1;
    int padding = 5;
    int* grid = new int[width * height];
    float cell_width = (float)window_width / width, cell_height = (float)window_height / height;


    for (int i = 0; i < width * height; i++) {
        grid[i] = i + 1;
    }

    grid[blank_i + blank_j * width] = 0;

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            cout << "DAMN";
        }

        for (int i = 0; i < width; i++) {
            for (int j = 0; j < height; j++) {
                int index = i + j * width;
                int val = grid[index];
                if (val == 0) continue;
                int x = i * cell_width;
                int y = j * cell_height;
                DrawRectangle(x + padding, y + padding, cell_width - padding * 2, cell_height - padding * 2, GREEN);
                //DrawCenteredText(TextFormat("(%i, %i) -> %i + %i * %i -> %i", i, j, i, j, width, index), x + cell_width / 2, y + cell_height / 2, 20, BLACK);
                //DrawCenteredText(TextFormat("%i -> (%i %% %i -> %i, %i / %i -> %i)", index, index, width, i, index, width, j), x + cell_width / 2, y + cell_height / 2, 20, BLACK);
                DrawCenteredText(TextFormat("%i", val), x + cell_width / 2, y + cell_height / 2, 40, BLACK);
            }
        }

        EndDrawing();
    }

    delete[] grid;
    CloseWindow();

    return 0;
}