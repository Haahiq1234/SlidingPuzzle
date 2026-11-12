#include "raylib.h"
#include <string.h>
#include <iostream>

using namespace std;

// tiles and cells mean the same thing
// screen space coordinates -> The coordinated on the screen (x, y)
// grid coordinates -> The coordinated on the grid (i, j)


class SlidingPuzzle {
public:
    SlidingPuzzle() = default;
    void Init(int w, int h); // H
    void Update(); // H
    void Render(); // H
    void Destroy(); // H
    void MouseButtonPress(); // H

private:
    int window_width, window_height;
    int width = 3, height = 3;
    int blank_i = width - 1, blank_j = height - 1; // The position of the blank tile
    float tile_width, tile_height; // the width and height of a single tile
    int padding = 5;
    int* grid;

    void screen_space_to_grid(int x, int y, int& i, int& j); //A: is obvious by the name (just divides x and y by the tile_width and tile_height)
    void grid_to_screen_space(int i, int j, int &x, int &y); //B: opposite of above function
    void get_mouse_pos(int &i, int &j); //A: Gets the position of the mouse using the raylib function 
    //  GetMousePosition, then converts it into the grid coordinates (hint: reverse of the method we used to convert grid to screen space coordinates)
    bool is_valid_move(int i, int j); //B: First checks if the new position is in bounds then checks if it is adjacent to the blank spot
    void pos(int index, int &i, int &j); //A: converts the grid index to the grid (i, j)
    int index(int i, int j); // B: Converts the grid (i, j) to the grid index
    void swap_position(int grid[], int i, int j); // A: Swaps the vlaue of tile (i, j) with the blank tile
    bool is_in_correct_place(int i, int j); // B: checks if the tile at (i, j) is in it's correct place. 
};



//using namespace std;

void DrawCenteredText(const char* cstr, int x, int y, int font_size, Color col) {
    DrawText(cstr, x - MeasureText(cstr, font_size) / 2, y - font_size / 2, font_size, col);
}

int main(void)
{
    int window_width = 800, window_height = 800;
    InitWindow(window_width, window_height, "raylib example - basic window");

    SlidingPuzzle game;

    game.Init(window_width, window_height);

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        game.Update();
        game.Render();
        EndDrawing();
    }
    game.Destroy();

    CloseWindow();

    return 0;
}

void SlidingPuzzle::Init(int w, int h)
{
    window_width = w;
    window_height = h;
    grid = new int[width * height];
    tile_width = (float)window_width / width;
    tile_height = (float)window_height / height;


    for (int i = 0; i < width * height; i++) {
        grid[i] = i + 1;
    }
    grid[blank_i + blank_j * width] = 0;
}

void SlidingPuzzle::Update()
{
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        MouseButtonPress();
}

void SlidingPuzzle::Render()
{
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            int idx = index(i, j);
            int val = grid[idx];
            if (val == 0) continue;
            int x, y;
            grid_to_screen_space(i, j, x, y);
            DrawRectangle(x + padding, y + padding, tile_width - padding * 2, tile_height - padding * 2, GREEN);
            //DrawCenteredText(TextFormat("(%i, %i) -> %i + %i * %i -> %i", i, j, i, j, width, index), x + cell_width / 2, y + cell_height / 2, 20, BLACK);
            //DrawCenteredText(TextFormat("%i -> (%i %% %i -> %i, %i / %i -> %i)", index, index, width, i, index, width, j), x + cell_width / 2, y + cell_height / 2, 20, BLACK);
            DrawCenteredText(TextFormat("%i", val), x + tile_width / 2, y + tile_height / 2, 40, BLACK);
        }
    }
}

void SlidingPuzzle::Destroy()
{
    delete[] grid;
}

void SlidingPuzzle::MouseButtonPress()
{
}

void SlidingPuzzle::grid_to_screen_space(int i, int j, int& x, int& y)
{
    x = i * tile_width;
    y = j * tile_height;
}

bool SlidingPuzzle::is_valid_move(int i, int j)
{
    if (i < 0 || i >= width || j < 0 || j >= height)
    {
        return false;
    }

    int distance_i = i - blank_i;
    int distance_j = j - blank_j;

    if (distance_i + distance_j == 1)
    {
        return true;
    }
}

int SlidingPuzzle::index(int i, int j)
{
    int result = i + j * width;
    return result;
}
