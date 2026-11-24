#include <cmath>
#include "raylib.h"
#include <vector>
#include <deque>
#include <iostream>
#include <string>
#include <fstream>

using namespace std;

#define MIN_N 3
#define MAX_N 6

#define GAME_STATE 0
#define MENU_STATE 1
#define SHUFFLING_STATE 2

#define IS_SHUFFLE_SLIDING true
#define SHUFFLE_SLIDE_DURATION 0.01
#define SLIDE_DURATION 0.1

#define BG_COL CLITERAL (Color) {100, 100, 100, 255}
#define CORRECT_COL CLITERAL (Color) {0, 230, 0, 255}
#define INCORRECT_COL CLITERAL (Color) {100, 230, 230, 255}

#define BUTTON_COL CLITERAL (Color) {150, 150, 150, 255}
#define HOV_BUTTON_COL CLITERAL (Color) {130, 130, 130, 255}
#define CLICKED_BUTTON_COL CLITERAL (Color) {120, 120, 120, 255}

// tiles and cells mean the same thing
// screen space coordinates -> The coordinated on the screen (x, y)
// grid coordinates -> The coordinated on the grid (i, j)


bool IsButtonPressed(int x, int y, int w, int h, const char *str, int font, Color reg_col, Color hovered_col, Color clicked_col, int mx, int my);
string FormatTime(int total_seconds);


class SlidingPuzzle {
public:
    SlidingPuzzle() = default;
    void Init(int w, int h); // H
    void Update(); // H
    void Render(); // H
    void Destroy(); // H

private:
    int game_mode = MENU_STATE;
    int panel_width = 600, panel_height = 600;
    int ox = -1, oy = -1;
    int width = 3, height = 3;
    int total_tiles = width * height;
    int blank_i = width - 1, blank_j = height - 1; // The position of the blank tile
    float tile_width = -1, tile_height = -1; // the width and height of a single tile
    int padding = 5;
    int* grid = nullptr;
    bool is_running = false;

    void BeginGame();
    void SetGameRunning();
    void ShuffleTiles(); // H
    void HandleInput(); // H

    void RenderTile(int x, int y, int value, bool is_correct = false); // H
    void RenderNumberedTile(int x, int y, int value, bool is_correct);


    void HandleMousePress(int x, int y); // A: Turn the x and y into the grid (i, j) check if it is a valid move, then use move_tile to make the move
    void CheckGameState(); // B: Loops over each tile in the grid and checks if it is in the correct position, and the last tile is 0 then if it is all correct it calls EndGame();
    void EndGame(); // A: Sets the last tile in the game back to what it was supposed to be grid position -> (width - 1, height - 1) value -> (width * height) then sets is_running as false

    void screen_space_to_grid(int x, int y, int& i, int& j); //A: is obvious by the name (just divides x and y by the tile_width and tile_height)
    void grid_to_screen_space(int i, int j, int& x, int& y); //B: opposite of above function
    bool is_valid_move(int i, int j); //B: First checks if the new position is in bounds then checks if it is adjacent to the blank spot
    void pos(int index, int& i, int& j); //A: converts the grid index to the grid (i, j)
    int index(int i, int j); // B: Converts the grid (i, j) to the grid index
    void move_tile(int i, int j, bool animate = false); // A: Swaps the vlaue of tile (i, j) with the blank tile
    bool is_in_correct_place(int i, int j); // B: checks if the tile at (i, j) is in it's correct place. 


    //IMAGES SECTION
    std::vector<Texture2D> textures;
    int current_tex = 0;
    void LoadImages();
    void LoadPuzzleImage(const char* name); // A: Load the texture with the name(filename) and add it to the vector textures
    void GetTileImageSource(int value, Rectangle& source);
    void RenderImagedTile(int x, int y, int value); // B: Get the source image rectangle and use it to render the tile.

    //ANIMATION SECTION
    bool is_animating = false;
    int moving_i = -1, moving_j = -1, moving_tile = -1;
    float anim_start_time = -1, anim_duration = 0.1; // In seconds
    deque<Vector2> move_queue;
    void Animate(int i, int j, int tile); // B: Begins the animation
    void UpdateAnimation();
    void EndAnimation(); // A: Ends the animation cycle

    //TIMER SECTION
    bool is_finished = false;
    double start_time = 0, end_time = 0;
    void UpdateAndRenderTimer();

    // SCORES SECTION
    vector<int> scores = { -1, 100, -1, -1 };
    void UpdateScore(int n, int secs);
    void LoadScores();
    void RenderScores();
    void SaveScores();
};



//using namespace std;

void DrawCenteredText(const char* cstr, int x, int y, int font_size, Color col) {
    DrawText(cstr, x - MeasureText(cstr, font_size) / 2, y - font_size / 2, font_size, col);
}

int main(void)
{
    srand(time(0));
    int window_width = 1200, window_height = 800;
    InitWindow(window_width, window_height, "Sliding Puzzle");

    SlidingPuzzle game;

    game.Init(window_width, window_height);

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(BG_COL);
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
    ox = (w - panel_width) / 2;
    oy = (h - panel_height) / 2;
    grid = new int[width * height];
    tile_width = (float)panel_width / width;
    tile_height = (float)panel_height / height;
    LoadImages();
    LoadScores();
    for (int i = 0; i < width * height; i++) {
        grid[i] = i + 1;
    }

    //BeginGame();
}

void SlidingPuzzle::Update()
{
    if (game_mode == MENU_STATE || !is_running) return;
    CheckGameState();
}

void SlidingPuzzle::Render()
{
    DrawRectangle(ox-padding, oy-padding, panel_width+padding * 2, panel_height + padding * 2, GRAY);
    HandleInput();
    if (is_animating) {
        UpdateAnimation();
        if (!is_animating && game_mode == SHUFFLING_STATE) {
            SetGameRunning();
            anim_duration = SLIDE_DURATION;
        }
    }
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            int idx = index(i, j);
            int val = grid[idx];
            if (val == 0) continue;
            int x, y;
            grid_to_screen_space(i, j, x, y);
            RenderTile(x, y, val, is_in_correct_place(i, j));
        };
    }
    if (!is_running && game_mode == GAME_STATE)
        DrawCenteredText("PUZZLE FINISHED!", ox + panel_width / 2, oy + panel_height / 2, 60, BLACK);
    UpdateAndRenderTimer();
    RenderScores();
}

void SlidingPuzzle::Destroy()
{
    delete[] grid;
}

void SlidingPuzzle::BeginGame()
{
    is_finished = false;
    for (int i = 0; i < width * height; i++) {
        grid[i] = i + 1;
    }
    grid[blank_i + blank_j * width] = 0;
    ShuffleTiles();
    if (IS_SHUFFLE_SLIDING)
        game_mode = SHUFFLING_STATE;
    else SetGameRunning();
}

void SlidingPuzzle::SetGameRunning()
{
        game_mode = GAME_STATE;
        is_running = true;
        start_time = GetTime();
}

void SlidingPuzzle::ShuffleTiles()
{
    int num_shuffles = (width * height) * 10;
    int curr_i = blank_i, curr_j = blank_j, prev_i = blank_i, prev_j = blank_j;
    if (IS_SHUFFLE_SLIDING) anim_duration = SHUFFLE_SLIDE_DURATION;
    while (--num_shuffles >= 0) {
        vector<Vector2> neighbours;
        if (curr_i > 0 && (curr_i - 1) != prev_i)
            neighbours.push_back({ (float)curr_i - 1, (float)curr_j });
        if (curr_j > 0 && (curr_j - 1) != prev_j)
            neighbours.push_back({ (float)curr_i, (float)curr_j - 1 });
        if (curr_i < width - 1 && (curr_i + 1) != prev_i)
            neighbours.push_back({ (float)curr_i + 1, (float)curr_j });
        if (curr_j < height - 1 && (curr_j + 1) != prev_j)
            neighbours.push_back({ (float)curr_i, (float)curr_j + 1 });

        Vector2 next = neighbours[rand() % neighbours.size()];
        prev_i = curr_i;
        prev_j = curr_j;

        curr_i = next.x;
        curr_j = next.y;

        move_tile(next.x, next.y, IS_SHUFFLE_SLIDING);
    }
}

void SlidingPuzzle::HandleInput()
{
    if (IsKeyPressed(KEY_A) && current_tex > 0)
        current_tex--;
    if (IsKeyPressed(KEY_D) && current_tex < (textures.size()))
        current_tex++;
    Vector2 mouse = GetMousePosition();
    if (game_mode== MENU_STATE) {
        if (IsButtonPressed(50, 50, 100, 100, "START", 20, BUTTON_COL, HOV_BUTTON_COL, CLICKED_BUTTON_COL, mouse.x, mouse.y)) {
            BeginGame();
        }
    } else if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        HandleMousePress(mouse.x, mouse.y);
    }
}

void SlidingPuzzle::RenderTile(int x, int y, int value, bool is_correct)
{
    if (current_tex == 0)
        RenderNumberedTile(x, y, value, is_correct);
    else
        RenderImagedTile(x, y, value);
}

void SlidingPuzzle::RenderNumberedTile(int x, int y, int value, bool is_correct)
{
    if (is_correct)
        DrawRectangle(ox + x + padding, oy + y + padding, tile_width - padding * 2, tile_height - padding * 2, CORRECT_COL);
    else
        DrawRectangle(ox + x + padding, oy + y + padding, tile_width - padding * 2, tile_height - padding * 2, INCORRECT_COL);
    //DrawCenteredText(TextFormat("(%i, %i) -> %i + %i * %i -> %i", i, j, i, j, width, index), x + cell_width / 2, y + cell_height / 2, 20, BLACK);
    //DrawCenteredText(TextFormat("%i -> (%i %% %i -> %i, %i / %i -> %i)", index, index, width, i, index, width, j), x + cell_width / 2, y + cell_height / 2, 20, BLACK);
    DrawCenteredText(TextFormat("%i", value),ox +  x + tile_width / 2, oy + y + tile_height / 2, 40, BLACK);
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

    int distance_i = abs(i - blank_i);
    int distance_j = abs(j - blank_j);

    if ((distance_i + distance_j) == 1)
    {
        return true;
    }
    return false;
}

int SlidingPuzzle::index(int i, int j)
{
    int result = i + j * width;
    return result;
}
void SlidingPuzzle::screen_space_to_grid(int x, int y, int& i, int& j)
{
    i = (x - ox) / tile_width;
    j = (y - oy) / tile_height;
}


void SlidingPuzzle::pos(int index, int& i, int& j)
{
    i = index % width;
    j = index / width;
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
void SlidingPuzzle::LoadImages()
{
    LoadPuzzleImage("res/puzzle.png");
    LoadPuzzleImage("res/flower.png");
}
void SlidingPuzzle::LoadPuzzleImage(const char* name)
{
    Texture2D tex = LoadTexture(name);
    textures.push_back(tex);
}
void SlidingPuzzle::GetTileImageSource(int value, Rectangle& source)
{
    int i, j;
    pos(value - 1, i, j);
    Texture2D tex = textures[current_tex - 1];
    source.width = (float)tex.width / width;
    source.height = (float)tex.height / height;
    source.x = i * source.width;
    source.y = j * source.height;
}
void SlidingPuzzle::UpdateAnimation()
{
    float current_time = GetTime();
    if (current_time > (anim_start_time + anim_duration)) {
        EndAnimation();
        return;
    }
    float t = (current_time - anim_start_time) / anim_duration;
    int x = (blank_i * (1 - t) + moving_i * t) * tile_width;
    int y = (blank_j * (1 - t) + moving_j * t) * tile_height;
    RenderTile(x, y, moving_tile);
}
void SlidingPuzzle::EndAnimation()
{
    int idx = index(moving_i, moving_j);
    grid[idx] = moving_tile;
    is_animating = false;
    if (!move_queue.empty())
    {
        Vector2 pos = move_queue.front();
        move_queue.pop_front();
        move_tile(pos.x, pos.y, true);
    }
}
void SlidingPuzzle::UpdateAndRenderTimer()
{
    float timer_width = 130;
    float timer_height = 40;
    float timer_x = ox + panel_width;
    float timer_y = oy  - timer_height * 1.5;
    DrawRectangle(timer_x, timer_y, timer_width, timer_height, GRAY);
    double current_time = (is_finished) ? end_time : GetTime();
    int total_seconds = (!is_finished && !is_running) ? 0 : floor(current_time - start_time);
    DrawCenteredText(FormatTime(total_seconds).c_str(), timer_x + timer_width / 2, timer_y + timer_height / 2, 30, BLACK);
}
void SlidingPuzzle::UpdateScore(int n, int secs)
{
    int i = n - MIN_N;
    if (scores[i] == -1 || secs < scores[i]) {
        scores[i] = secs;
        SaveScores();
    }
}
void SlidingPuzzle::LoadScores()
{
    ifstream file("scores.txt");
    scores = {};
    if (!file.is_open()) {
        for (int i = MIN_N; i <= MAX_N; i++) 
            scores.push_back(-1);
        SaveScores();
    }
    else {
        for (int i = MIN_N; i <= MAX_N; i++) {
            int score;
            file >> score;
            scores.push_back(score);
        }
        file.close();
    }
}
void SlidingPuzzle::RenderScores()
{
    int font_size = 26;
    int score_w = 200;
    int score_h = 40;
    int score_x = ox + panel_width + 20;
    int score_y = oy + 40;
    DrawRectangle(score_x, score_y, score_w, score_h, GRAY);
    DrawCenteredText("LOWEST", score_x+score_w/2, score_y+score_h/2, font_size, BLACK);
    for (int i = 0; i < scores.size(); i++) {
        DrawRectangle(score_x, score_y + (i+1) * (score_h + 10), score_w, score_h, GRAY);
        string str = to_string(i + MIN_N) + " by " + to_string(i + MIN_N) + " :: ";
        if (scores[i] == -1) str += "NONE";
        else str += FormatTime(scores[i]);
        DrawText(str.c_str(), score_x+5, score_y + (i+1) * (score_h + 10) +score_h * 0.5 - font_size/2, font_size, BLACK);
    }
}
void SlidingPuzzle::SaveScores()
{
    ofstream file("scores.txt");
    for (int i = 0; i < scores.size(); i++) {
        file << scores[i] << '\n';
    }
}
void SlidingPuzzle::CheckGameState()//checks each tile if it is in correct place if it is then ends the game other wise do not end.
{
    int count = 0;
    for (int i = 0;i < width;i++)
    {
        for (int j = 0;j < height;j++)
        {
            if (is_in_correct_place(i, j))
            {
                count++;
            }
        }
    }
    if (count == total_tiles - 1)
    {
        EndGame();
    }
}
void SlidingPuzzle::move_tile(int i, int j, bool animate)
{
    if (is_animating && animate) {
        move_queue.push_back({ (float)i, (float)j });
        return;
    }
    int position1, temp, position2;
    position1 = index(i, j);
    position2 = index(blank_i, blank_j);
    temp = grid[position1];
    grid[position1] = grid[position2];
    grid[position2] = temp;
    int pi = blank_i, pj = blank_j;
    blank_i = i;
    blank_j = j;
    if (animate)
        Animate(pi, pj, grid[position2]);
}
void SlidingPuzzle::HandleMousePress(int x, int y)
{
    int i, j;
    screen_space_to_grid(x, y, i, j);
    if (is_valid_move(i, j)) {
        move_tile(i, j, true);
    }
}
void SlidingPuzzle::EndGame()
{
    int x = index(blank_i, blank_j);
    grid[x] = x + 1;
    is_running = false;
    is_finished = true;
    end_time = GetTime();
    game_mode = MENU_STATE;
    UpdateScore(width, floor(end_time-start_time));
}
bool IsButtonPressed(int x, int y, int w, int h, const char* str, int font , Color reg_col, Color hovered_col, Color clicked_col, int mx, int my)
{
    bool is_hovered = (mx >= x && my >= y && mx <= (x + w) && my <= (y + h));
    bool is_pressed = is_hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
    if (is_hovered && IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        DrawRectangle(x, y, w, h, clicked_col);
    else if (is_hovered)
        DrawRectangle(x, y, w, h, hovered_col);
    else
        DrawRectangle(x, y, w, h, reg_col);
    DrawCenteredText(str, x+w/2, y+h/2, font, BLACK);
    return is_pressed;
}
string FormatTime(int total_seconds)
{
    int num_sec = total_seconds % 60;
    int num_min = total_seconds / 60;
    if (num_min >= 10) {
        num_min = 9;
        num_sec = 59;
    }
    string timestr = to_string(num_min) + ":";
    if (num_sec < 10) timestr += '0';
    timestr += to_string(num_sec);
    return timestr;
}
void SlidingPuzzle::RenderImagedTile(int x, int y, int value)
{
    Rectangle src;
    GetTileImageSource(value, src);
    DrawTexturePro(textures[current_tex - 1], src, { (float)(x+ox), (float)(y+oy), tile_width, tile_height}, {0, 0}, 0, WHITE);
}
void SlidingPuzzle::Animate(int i, int j, int tile)
{
    moving_i = i;
    moving_j = j;

    anim_start_time = GetTime();
    is_animating = true;
    moving_tile = tile;
    grid[index(i, j)] = 0;
}