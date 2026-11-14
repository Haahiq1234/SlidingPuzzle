#include "raylib.h"
#include <string.h>
#include <iostream>
#include <vector>
#include <random>

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

private:
    int window_width = -1, window_height = -1;
    int width = 3, height = 3;
    int blank_i = width - 1, blank_j = height - 1; // The position of the blank tile
    float tile_width = -1, tile_height = -1; // the width and height of a single tile
    int padding = 5;
    int* grid = nullptr;
    bool is_running = false;

    void BeginGame();
    void ShuffleTiles(); // H
    void HandleInput(); // H
    void RenderTile(int x, int y, int value, bool is_correct = false); // H
    void HandleMousePress(int x, int y); // A: Turn the x and y into the grid (i, j) check if it is a valid move, then use move_tile to make the move
    void CheckGameState(); // B: Loops over each tile in the grid and checks if it is in the correct position, and the last tile is 0 then if it is all correct it calls EndGame();
    void EndGame(); // A: Sets the last tile in the game back to what it was supposed to be grid position -> (width - 1, height - 1) value -> (width * height) then sets is_running as false

    void screen_space_to_grid(int x, int y, int& i, int& j); //A: is obvious by the name (just divides x and y by the tile_width and tile_height)
    void grid_to_screen_space(int i, int j, int &x, int &y); //B: opposite of above function
    void get_mouse_pos(int &i, int &j); //A: Gets the position of the mouse using the raylib function 
    //  GetMousePosition, then converts it into the grid coordinates (hint: reverse of the method we used to convert grid to screen space coordinates)
    bool is_valid_pos(int i, int j); // B: Just checks whether the (i, j) are within bounds
    bool is_valid_move(int i, int j); //B: First checks if the new position is in bounds then checks if it is adjacent to the blank spot
    void pos(int index, int &i, int &j); //A: converts the grid index to the grid (i, j)
    int index(int i, int j); // B: Converts the grid (i, j) to the grid index
    void move_tile(int i, int j); // A: Swaps the vlaue of tile (i, j) with the blank tile
    bool is_in_correct_place(int i, int j); // B: checks if the tile at (i, j) is in it's correct place. 
};



//using namespace std;

void DrawCenteredText(const char* cstr, int x, int y, int font_size, Color col) {
    DrawText(cstr, x - MeasureText(cstr, font_size) / 2, y - font_size / 2, font_size, col);
}

int main(void)
{
    srand(time(0));
    int window_width = 800, window_height = 800;
    InitWindow(window_width, window_height, "Sliding Puzzle");

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

    BeginGame();
}

void SlidingPuzzle::Update()
{
    HandleInput();
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
            RenderTile(x, y, val, is_in_correct_place(i, j));
        }
    }
}

void SlidingPuzzle::Destroy()
{
    delete[] grid;
}

void SlidingPuzzle::BeginGame()
{
    for (int i = 0; i < width * height; i++) {
        grid[i] = i + 1;
    }
    grid[blank_i + blank_j * width] = 0;
    ShuffleTiles();
}

void SlidingPuzzle::ShuffleTiles()
{
    int num_shuffles = (width * height) * 10;
    int curr_i  =blank_i, curr_j = blank_j, prev_i = blank_i, prev_j = blank_j;
    while (--num_shuffles >= 0) {
        vector<Vector2> neighbours;
        if (curr_i > 0 && (curr_i - 1) != prev_i)
            neighbours.push_back({ (float)curr_i - 1, (float)curr_j });
        if (curr_j > 0 && (curr_j - 1) != prev_j)
            neighbours.push_back({ (float)curr_i, (float)curr_j -1});
        if (curr_i < width-1 && (curr_i + 1) != prev_i)
            neighbours.push_back({ (float)curr_i + 1, (float)curr_j });
        if (curr_j < height - 1 && (curr_j + 1) != prev_j)
            neighbours.push_back({ (float)curr_i, (float)curr_j +1});

        Vector2 next = neighbours[rand() % neighbours.size()];
        prev_i = curr_i;
        prev_j = curr_j;

        curr_i = next.x;
        curr_j = next.y;
        
        move_tile(next.x, next.y);
    }
}

void SlidingPuzzle::HandleInput()
{
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        Vector2 mouse = GetMousePosition();
        //HandleMousePress(mouse.x, mouse.y);
    }
}

void SlidingPuzzle::RenderTile(int x, int y, int value, bool is_correct)
{
    if (is_correct)
        DrawRectangle(x + padding, y + padding, tile_width - padding * 2, tile_height - padding * 2, GREEN);
    else 
        DrawRectangle(x + padding, y + padding, tile_width - padding * 2, tile_height - padding * 2, BLUE);
    //DrawCenteredText(TextFormat("(%i, %i) -> %i + %i * %i -> %i", i, j, i, j, width, index), x + cell_width / 2, y + cell_height / 2, 20, BLACK);
    //DrawCenteredText(TextFormat("%i -> (%i %% %i -> %i, %i / %i -> %i)", index, index, width, i, index, width, j), x + cell_width / 2, y + cell_height / 2, 20, BLACK);
    DrawCenteredText(TextFormat("%i", value), x + tile_width / 2, y + tile_height / 2, 40, BLACK);
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
void SlidingPuzzle::screen_space_to_grid(int x, int y, int& i, int& j)
{
    i = x / tile_width;
    j = y / tile_height;
}

void SlidingPuzzle::get_mouse_pos(int& i, int& j)
{
    int x, y;
    Vector2 pos = GetMousePosition();
    x = pos.x;
    y = pos.y;
    screen_space_to_grid(x, y, i, j);
}

void SlidingPuzzle::pos(int index, int& i, int& j)
{
    i = index / width;
    j = index % height;
}
//if value of grid at index = index+1 return true return false
bool SlidingPuzzle::is_in_correct_place(int i, int j)
{
    int idx = index(i, j);
    if (grid[idx] == idx + 1)
        return true;
    else
        return false;
}
void SlidingPuzzle::move_tile(int i, int j)
{
    int position1, temp, position2;
    position1 = index(i, j);
    position2 = index(blank_i, blank_j);
    temp = grid[position1];
    grid[position1] = grid[position2];
    grid[position2] = temp;
    blank_i = i;
    blank_j = j;
}
