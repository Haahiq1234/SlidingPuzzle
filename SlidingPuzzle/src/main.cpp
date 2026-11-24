#include <cmath>
#include "raylib.h"
#include <vector>
#include <deque>
#include <queue>
#include <iostream>
#include <string>
#include <unordered_set>
#include <fstream>
#define Grd std::vector<int>


using namespace std;

//#define log_grid(grid, n) for (int i = 0; i < (n * n); i++) {cout << grid[i] << '\t'; if ((i+1)%n==0) cout << '\n';}

#define MIN_N 3
#define MAX_N 6

#define GAME_STATE 0
#define MENU_STATE 1
#define SHUFFLING_STATE 2
#define ENDED_STATE 3

#define IS_SHUFFLE_SLIDING true
#define SHUFFLE_SLIDE_DURATION 0.01
#define SLIDE_DURATION 0.1

#define BG_COL CLITERAL (Color) {100, 100, 100, 255}
#define CORRECT_COL CLITERAL (Color) {0, 230, 0, 255}
#define INCORRECT_COL CLITERAL (Color) {100, 230, 230, 255}

#define LABEL_COL CLITERAL (Color) {100, 150, 100, 255}

#define BUTTON_COL CLITERAL (Color) {150, 150, 150, 255}
#define HOV_BUTTON_COL CLITERAL (Color) {130, 130, 130, 255}
#define CLICKED_BUTTON_COL CLITERAL (Color) {120, 120, 120, 255}

#define LABEL_WIDTH 200
#define LABEL_HEIGHT 40

// tiles and cells mean the same thing
// screen space coordinates -> The coordinated on the screen (x, y)
// grid coordinates -> The coordinated on the grid (i, j)

#define ROT_0 0
#define ROT_90 1
#define ROT_180 2
#define ROT_270 3


bool IsButtonPressed(float x, float y, float w, float h, const char* str, int font, Color reg_col, Color hovered_col, Color clicked_col);
bool IsArrowButtonPressedIm(float x, float y, float w, float h, Texture2D tex, int rot_mode);
string FormatTime(int total_seconds);
vector<Vector2> solve_grid(int* grid, int n, int blank_i, int blank_j);
bool is_path_solver(int* grid, int n, vector<Vector2> path, int blank_i, int blank_j);

class SlidingPuzzle {
public:
    SlidingPuzzle() = default;
    void Init(int w, int h); // H
    void Update(); // H
    void Render(); // H
    void Destroy(); // H

    bool ShouldExit();

private:
    int game_mode = MENU_STATE;
    int panel_width = 600, panel_height = 600;
    int ox = -1, oy = -1;
    int width = 3, height = 3;
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
    void move_tile(int i, int j, bool animate = false); // A: Swaps the value of tile (i, j) with the blank tile
    bool is_in_correct_place(int i, int j); // B: checks if the tile at (i, j) is in it's correct place.


    //IMAGES SECTION
    std::vector<Texture2D> textures;
    int current_tex = 0;
    void LoadGameTextures();
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
    vector<int> scores;
    void UpdateScore(int n, int secs);
    void LoadScores();
    void RenderScores();
    void SaveScores();

    // SIZE MODES
    void SetMode(int n);
    void RenderModes();

    // APPEARANCE
    Texture2D arrow_button;
    void RenderAppearanceMenu();

    // SOUNDS
    Sound sliding_sound;
    Music sound_track;
    bool is_muted;
    Texture2D muted, unmuted;
    void LoadSounds();
    void PlaySlidingSound();
    void UpdateGameMusic();
    void RenderMusicButton();
    void ToggleMusic();

    // SELF SOLVE
    bool autosolved = false;
    void RenderSelfSolveMenu();

    // ENDING SCREEN
    bool is_exited = false;
    void RenderEndMenu();
};


int main(void)
{
    srand(time(0));
    int window_width = 1200, window_height = 800;
    InitWindow(window_width, window_height, "Sliding Puzzle");

    SlidingPuzzle game;

    InitAudioDevice();
    game.Init(window_width, window_height);

    while (!WindowShouldClose() && !game.ShouldExit())
    {
        BeginDrawing();
        ClearBackground(BG_COL);
        game.Update();
        game.Render();
        EndDrawing();
    }
    game.Destroy();
    CloseAudioDevice();
    CloseWindow();

    return 0;
}

//using namespace std;

void DrawCenteredText(const char* cstr, int x, int y, int font_size, Color col) {
    DrawText(cstr, x - MeasureText(cstr, font_size) / 2, y - font_size / 2, font_size, col);
}
class State {
public:
    int i, j, n, depth, reward;
    vector<int> grid;
    vector<Vector2> path;
    State(int i, int j, int n, int* grid, int _depth);
    void Swap(int blank_i, int blank_j);
    bool is_solved();
};

struct GrdHash
{
    inline size_t operator()(const Grd& st) const
    {
        std::hash<int> hasher;
        size_t seed = 0;
        int j = 0;
        for (int i : st)
        {
            seed ^= hasher(i) + j + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            j++;
        }
        return seed;
    }
};
struct CompareState
{
    inline bool operator()(const State& a, const State& b)
    {
        return a.reward < b.reward;
    }
};


//vector<Vector2> solve_grid(int* grid, int n);

int calculate_reward(Grd grid, int n, int depth);


vector<Vector2> solve_grid(int* grid, int n, int blank_i, int blank_j) {
    vector<Vector2> solution;


    State start = State(blank_i, blank_j, n, grid, 0);
    unordered_set<Grd, GrdHash> visited;
    std::priority_queue<State, vector<State>, CompareState> queue;

    Vector2 dirs[] = { { 1, 0 },{ 0, 1 },{ -1, 0 },{ 0, -1 } };

    queue.push(start);
    visited.insert(start.grid);
    /*visited.insert(previous.grid);

    if (!(previous == start))
        previous = start;*/

    State best = start;

    int counter = 1;

    // start.log_state();

    while (!queue.empty() && counter < 1000000) // visited.size() < 100 &&
    {
        // cout << n << ' ';
        State current = queue.top();
        queue.pop();
        if (current.depth > 10 * (n * n)) // 2.5 * (width + height))
            continue;

        if (current.is_solved())
        {
            best = current;
            // break;
            // return;
            for (Vector2 p : current.path)
            {
                //cout << "(" << p.x << ", " << p.y << ") -> ";
                solution.push_back(p);
            }
            //cout << "Victory after " << (visited.size() + queue.size()) << " checks!\n";
            return solution;
        }
        // if (visited.find(current) != visited.end())
        // {
        //     current.grid = {};
        //     continue;
        // }
        // current.log_state();
        for (int i = 0; i < 4; i++)
        {
            int ni = current.i + dirs[i].x;
            int nj = current.j + dirs[i].y;
            if (ni < 0 || nj < 0 || ni >= n || nj >= n)
                continue;

            State next = State(ni, nj, n, current.grid.data(), current.depth + 1);
            // cout << current.depth << ' ' << next.depth << '\n';
            next.path.assign(current.path.begin(), current.path.end());
            next.path.push_back({ (float)ni, (float)nj });
            next.Swap(current.i, current.j);
            next.reward = calculate_reward(next.grid, n, next.depth);
            if (visited.find(next.grid) != visited.end())
            {
                // cout << "Conflict arisen: " << visited.find(next)->reward << ' ' << next.reward << '\n';
                continue;
            }
            visited.insert(next.grid);
            if (next.reward > best.reward || (next.reward == best.reward && next.depth < best.depth))
                best = next;
            // cout << "Grid Hash: " << StateHash()(next) << '\n';
            // next.log_state();
            queue.push(next);
            counter++;
        }
    }

    //cout << "(" << best.path[0].x << ", " << best.path[0].y << ") -> " << best.depth << ' ' << visited.size() << "\n";
    return { { (float)best.path[0].x, (float)best.path[0].y } };
}


State::State(int _i, int _j, int _n, int* _grid, int _depth)
{
    i = _i;
    j = _j;
    n = _n;
    grid = vector<int>(_grid, _grid + n * n);
    depth = _depth;
}

void State::Swap(int blank_i, int blank_j)
{
    int ind_1 = blank_i + blank_j * n;
    int ind_2 = i + j * n;
    int temp = grid[ind_1];
    grid[ind_1] = grid[ind_2];
    grid[ind_2] = temp;

}

bool State::is_solved()
{
    for (int i = 0; i < grid.size() - 1; i++)
        if (grid[i] != (i + 1))
            return false;
    return true;
}

int calculate_reward(Grd grid, int n, int depth)
{
    int reward = 0;
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            int val = grid[i + j * n];
            if (val == 0)
                val = n * n;
            int vi = (val - 1) % n, vj = (val - 1) / n;
            int dx = (vi - i), dy = vj - j;
            if (dx < 0)
                dx = -dx;
            if (dy < 0)
                dy = -dy;
            reward -= (dx + dy) * (n * n - val);
        }
    }
    reward = reward * 1000 - depth;

    for (int i = 0; i < (n * n); i++)
    {
        // cout << depth << ' ' << reward << ' ';
        if (grid[i] != (i + 1))
        {
            for (int j = i + 1; j < (n * n); j++)
            {
                if (grid[j] == (i + 1))
                {
                    int x = (i % n) - (j % n);
                    int y = (i / n) - (j / n);
                    if (x < 0)
                        x = -x;
                    if (y < 0)
                        y = -y;
                    reward -= (x + y);
                }
            }
            break;
            // cout << "\n";
            //turn false;
        }
        reward += 10000;
    }
    return reward;
}

void SlidingPuzzle::Init(int w, int h)
{
    ox = (w - panel_width) / 2;
    oy = (h - panel_height) / 2;
    grid = new int[width * height];
    tile_width = (float)panel_width / width;
    tile_height = (float)panel_height / height;
    LoadGameTextures();
    LoadSounds();
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
    DrawRectangle(ox - padding, oy - padding, panel_width + padding * 2, panel_height + padding * 2, GRAY);
    UpdateGameMusic();
    HandleInput();
    if (is_animating) {
        UpdateAnimation();
        if (!is_animating && game_mode == SHUFFLING_STATE) {
            SetGameRunning();
            anim_duration = SLIDE_DURATION;
        }
    }
    if (game_mode == MENU_STATE) {
        if (IsButtonPressed(ox - LABEL_WIDTH - 20, oy + panel_height - LABEL_HEIGHT * 2, LABEL_WIDTH, LABEL_HEIGHT * 2, "START", 40, BUTTON_COL, HOV_BUTTON_COL, CLICKED_BUTTON_COL))
            BeginGame();
    }
    else if (game_mode == GAME_STATE) {

        if (IsButtonPressed(ox + panel_width + 20, oy + panel_height - LABEL_HEIGHT * 2, LABEL_WIDTH, LABEL_HEIGHT * 2, "RESET", 40, BUTTON_COL, HOV_BUTTON_COL, CLICKED_BUTTON_COL)) {
            for (int i = 0; i < (width * height); i++) {
                grid[i] = i + 1;
            }
            blank_i = width - 1;
            blank_j = height - 1;
            EndGame();
            game_mode = MENU_STATE;
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
    UpdateAndRenderTimer();
    if (game_mode != ENDED_STATE) {

        if (!is_running && game_mode == GAME_STATE)
            DrawCenteredText("PUZZLE FINISHED!", ox + panel_width / 2, oy + panel_height / 2, 60, BLACK);
        RenderScores();
        RenderModes();
        if (game_mode == MENU_STATE)
            RenderAppearanceMenu();
        if (width == 3 && game_mode == GAME_STATE)
            RenderSelfSolveMenu();
    }
    else {
        RenderEndMenu();
    }
    RenderMusicButton();
}

void SlidingPuzzle::Destroy()
{
    for (int i = 0; i < textures.size(); i++) {
        UnloadTexture(textures[i]);
    }
    UnloadMusicStream(sound_track);
    delete[] grid;
}

bool SlidingPuzzle::ShouldExit()
{
    return is_exited;
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
    autosolved = false;
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
    Vector2 mouse = GetMousePosition();
    if (game_mode==GAME_STATE && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        HandleMousePress(mouse.x, mouse.y);
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
    DrawCenteredText(TextFormat("%i", value), ox + x + tile_width / 2, oy + y + tile_height / 2, 40, BLACK);
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
    i = floor((x - ox) / tile_width);
    j = floor((y - oy) / tile_height);
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
void SlidingPuzzle::LoadGameTextures()
{
    LoadPuzzleImage("res/puzzle.png");
    LoadPuzzleImage("res/kitten.png");
    LoadPuzzleImage("res/flower.png");
    LoadPuzzleImage("res/image_3.png");
    LoadPuzzleImage("res/image_4.png");
    LoadPuzzleImage("res/image_5.png");
    LoadPuzzleImage("res/image_6.png");
    LoadPuzzleImage("res/image_7.png");
    LoadPuzzleImage("res/image_8.png");
    LoadPuzzleImage("res/image_9.png");
    LoadPuzzleImage("res/image_10.png");
    LoadPuzzleImage("res/image_2.png");
    LoadPuzzleImage("res/image_1.png");

    muted = LoadTexture("res/muted.png");
    unmuted = LoadTexture("res/unmuted.png");
    arrow_button = LoadTexture("res/button.png");
}
void SlidingPuzzle::LoadPuzzleImage(const char* name)
{
    Texture2D tex = LoadTexture(name);
    if (IsTextureValid(tex))
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

    float timer_width = LABEL_WIDTH + 10;
    float timer_height = LABEL_HEIGHT + 10;
    /*float timer_x = ox + panel_width + 20;
    float timer_y = oy + (scores.size() + 2) * (timer_height)+30;*/

    float timer_x = ox - timer_width - 20;//(panel_width - timer_width)/2;
    float timer_y = oy + 40 + (MAX_N - MIN_N + 2) * (timer_height)+20;
    //cout << timer_x << timer_y << timer_width << timer_height;

    DrawRectangle(timer_x, timer_y, timer_width, timer_height, GRAY);
    
    double current_time = (is_finished) ? end_time : GetTime();
    int total_seconds = (!is_finished && !is_running) ? 0 : floor(current_time - start_time);
    
    DrawCenteredText(("TIMER: " + FormatTime(total_seconds)).c_str(), timer_x + timer_width / 2, timer_y + timer_height / 2, 30, BLACK);
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
    while (scores.size() <= (MAX_N - MIN_N)) scores.push_back(-1);
}
void SlidingPuzzle::RenderScores()
{
    int font_size = 26;
    int score_w = LABEL_WIDTH;
    int score_h = LABEL_HEIGHT;
    int score_x = ox + panel_width + 20;
    int score_y = oy + 40;
    int p = 4;
    DrawRectangle(score_x - p, score_y - p, score_w + p * 2, score_h + p * 2, LABEL_COL);
    DrawCenteredText("LOWEST", score_x + score_w / 2, score_y + score_h / 2, font_size + 5, BLACK);
    for (int i = 0; i < scores.size(); i++) {
        DrawRectangle(score_x, score_y + (i + 1) * (score_h + 10), score_w, score_h, GRAY);
        string str = to_string(i + MIN_N) + " by " + to_string(i + MIN_N) + " :: ";
        if (scores[i] == -1) str += "NONE";
        else str += FormatTime(scores[i]);
        DrawText(str.c_str(), score_x + 5, score_y + (i + 1) * (score_h + 10) + score_h * 0.5 - font_size / 2, font_size, BLACK);
    }
}
void SlidingPuzzle::SaveScores()
{
    ofstream file("scores.txt");
    for (int i = 0; i < scores.size(); i++) {
        file << scores[i] << '\n';
    }
}
void SlidingPuzzle::SetMode(int n)
{
    width = n;
    height = n;
    grid = new int[n * n];
    for (int i = 0; i < (n * n); i++) {
        grid[i] = i + 1;
    }
    tile_height = (float)panel_height / height;
    tile_width = (float)panel_width / width;
    blank_i = blank_j = width - 1;
}
void SlidingPuzzle::RenderModes()
{
    int font_size = 26;
    int mode_w = LABEL_WIDTH;
    int mode_h = LABEL_HEIGHT;
    int mode_x = ox - 20 - mode_w;
    int mode_y = oy + 40;
    int p = 4;
    DrawRectangle(mode_x - p, mode_y - p, mode_w + p * 2, mode_h + p * 2, LABEL_COL);
    DrawCenteredText("MODES", mode_x + mode_w / 2, mode_y + mode_h / 2, font_size + 10, BLACK);
    for (int n = MIN_N; n <= MAX_N; n++) {
        int i = n - MIN_N;
        int y = mode_y + (i + 1) * (mode_h + 10);
        string str = to_string(n) + " by " + to_string(n);
        if (game_mode == MENU_STATE) {

            if (IsButtonPressed(mode_x, y, mode_w, mode_h, str.c_str(), font_size, (n != width) ? BUTTON_COL : HOV_BUTTON_COL, HOV_BUTTON_COL, CLICKED_BUTTON_COL)) {
                SetMode(n);
            }
        }
        else {
            DrawRectangle(mode_x, y, mode_w, mode_h, (n == width) ? BUTTON_COL : HOV_BUTTON_COL);
            DrawCenteredText(str.c_str(), mode_x + mode_w / 2, y + mode_h / 2, font_size, BLACK);
        }
    }
}
void SlidingPuzzle::RenderAppearanceMenu()
{
    int label_width = LABEL_WIDTH;
    int label_height = LABEL_HEIGHT;
    int button_size = label_height;
    int x = ox + (panel_width - label_width) / 2;
    int y = oy + panel_height + 20;
    DrawRectangle(x, y, label_width, label_height, LABEL_COL);
    DrawCenteredText("APPEARANCE", x + label_width / 2, y + label_height / 2, 26, BLACK);


    if (IsArrowButtonPressedIm(x - button_size * 1.5, y, button_size, button_size, arrow_button, ROT_180)) {
        if (current_tex > 0) current_tex--;
        //cout << "DAMN";
    }
    if (IsArrowButtonPressedIm(x + button_size * 0.5 + label_width, y, button_size, button_size, arrow_button, ROT_0)) {
        if (current_tex < textures.size()) current_tex++;
        //cout << "DAMN";
    }
}
void SlidingPuzzle::LoadSounds()
{
    sound_track = LoadMusicStream("res/track.mp3");
    sliding_sound = LoadSound("res/click_3.wav");
    PlayMusicStream(sound_track);
}
void SlidingPuzzle::PlaySlidingSound()
{
    PlaySound(sliding_sound);
}
void SlidingPuzzle::UpdateGameMusic()
{
    if (!is_muted)
    UpdateMusicStream(sound_track);
}
void SlidingPuzzle::RenderMusicButton()
{
    int button_width = 50;
    int button_height = 50;
    int button_x = 20;
    int button_y = 20;
    if (IsArrowButtonPressedIm(button_x, button_y, button_width, button_height, (is_muted) ? muted : unmuted, ROT_0)) {
        ToggleMusic();
    }
}
void SlidingPuzzle::ToggleMusic()
{
    if (is_muted) {
        ResumeMusicStream(sound_track);
    }
    else {
        PauseMusicStream(sound_track);
    }
    is_muted = !is_muted;
}
void SlidingPuzzle::RenderSelfSolveMenu()
{
    if (!autosolved && IsButtonPressed(ox - LABEL_WIDTH - 20, oy + panel_height - LABEL_HEIGHT * 5, LABEL_WIDTH, LABEL_HEIGHT * 2, "SOLVE", 40, BUTTON_COL, HOV_BUTTON_COL, CLICKED_BUTTON_COL)) {
        vector<Vector2> path = solve_grid(grid, width, blank_i, blank_j);
        if (is_path_solver(grid, width, path, blank_i, blank_j))
            autosolved = true;
        for (int i = 0; i < path.size(); i++) {
            move_tile(path[i].x, path[i].y, true);
        }
    }
}
void SlidingPuzzle::RenderEndMenu()
{
    // tl stands for top label
    int tl_w = 500;
    int tl_h = 60;
    int tl_x = ox + (panel_width - tl_w) / 2;
    int tl_y = 20;
    DrawRectangle(tl_x, tl_y, tl_w, tl_h, LABEL_COL);
    DrawCenteredText("!!  PUZZLE COMPLETED  !!", tl_x + tl_w / 2, tl_y + tl_h / 2, 35, BLACK);

    int b_width = LABEL_WIDTH;
    int b_height = LABEL_HEIGHT * 2;
    int b_y = oy + panel_height - b_height;

    if (IsButtonPressed(ox - LABEL_WIDTH - 20, b_y, b_width, b_height, "TRY AGAIN", 32, BUTTON_COL, HOV_BUTTON_COL, CLICKED_BUTTON_COL)) {
        BeginGame();
    }
    else if (IsButtonPressed(ox + panel_width + 20, b_y, b_width, b_height, "EXIT", 35, BUTTON_COL, HOV_BUTTON_COL, CLICKED_BUTTON_COL)) {
        is_exited = true;
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
    if (count == (width * height) - 1)
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
    if (!is_valid_move(i, j)) {
        if (!move_queue.empty()) {
            Vector2 v = move_queue.front();
            move_tile(v.x, v.y, true);
            move_queue.pop_front();
        }
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
    if (animate) {
        Animate(pi, pj, grid[position2]);
        //if (game_mode == GAME_STATE) {
        PlaySlidingSound();
        //}
    }
}
void SlidingPuzzle::HandleMousePress(int x, int y)
{
    int i, j;
    screen_space_to_grid(x, y, i, j);
    move_tile(i, j, true);
}
void SlidingPuzzle::EndGame()
{
    int x = index(blank_i, blank_j);
    grid[x] = x + 1;
    is_running = false;
    is_finished = true;
    end_time = GetTime();
    game_mode = ENDED_STATE;
    if (!autosolved)
        UpdateScore(width, floor(end_time - start_time));
}
bool IsButtonPressed(float x, float y, float w, float h, const char* str, int font, Color reg_col, Color hovered_col, Color clicked_col)
{
    Vector2 mouse = GetMousePosition();
    int mx = mouse.x, my = mouse.y;
    bool is_hovered = (mx >= x && my >= y && mx <= (x + w) && my <= (y + h));
    bool is_pressed = is_hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
    if (is_hovered && IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        DrawRectangleRounded({ x, y, w, h }, 0.5f, 5, clicked_col);
    else if (is_hovered)
        DrawRectangleRounded({ x, y, w, h }, 0.5f, 5, hovered_col);
    else
        DrawRectangleRounded({ x, y, w, h }, 0.5f, 5, reg_col);
    DrawCenteredText(str, x + w / 2, y + h / 2, font, BLACK);
    return is_pressed;
}
bool IsArrowButtonPressedIm(float x, float y, float w, float h, Texture2D tex, int rot_mode)
{
    Vector2 mouse = GetMousePosition();
    int mx = mouse.x, my = mouse.y;
    bool is_hovered = (mx >= x && my >= y && mx <= (x + w) && my <= (y + h));
    bool is_pressed = is_hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
    Color tint = { 200, 200, 200, 255 };
    if (is_hovered && IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        tint = GRAY;
    else if (is_hovered)
        tint = { 160, 160, 160, 255 };
    if (rot_mode == ROT_90) x += w;
    if (rot_mode == ROT_180) { x += w;y += h; };
    if (rot_mode == ROT_270) y += h;
    DrawTexturePro(tex, { 0, 0, (float)tex.width, (float)tex.height }, { x, y, w, h }, { 0, 0 }, rot_mode * 90, tint);
    return is_pressed;
}
string FormatTime(int total_seconds)
{
    int num_sec = total_seconds % 60;
    int num_min = total_seconds / 60;
    
    if (num_min >= 100) {
        num_min = 99;
        num_sec = 59;
    }

    string timestr;
    if (num_min < 10) timestr += '0';
    timestr += to_string(num_min) + ":";
    if (num_sec < 10) timestr += '0';
    timestr += to_string(num_sec);
    return timestr;
}
void SlidingPuzzle::RenderImagedTile(int x, int y, int value)
{
    Rectangle src;
    GetTileImageSource(value, src);
    DrawTexturePro(textures[current_tex - 1], src, { (float)(x + ox), (float)(y + oy), tile_width, tile_height }, { 0, 0 }, 0, WHITE);
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

bool is_path_solver(int* grid, int n, vector<Vector2> path, int blank_i, int blank_j)
{
    int* grid_2 = new int[n * n];
    for (int i = 0; i < (n * n); i++)
        grid_2[i] = grid[i];

    int pi = blank_i, pj = blank_j, p_index = blank_i + blank_j * n;
    for (int i = 0; i < path.size(); i++) {
        int ni = path[i].x;
        int nj = path[i].y;
        int n_ind = ni + nj * n;

        int temp = grid_2[n_ind];
        grid_2[n_ind] = grid_2[p_index];
        grid_2[p_index] = temp;
        
        pi = ni;
        pj = nj;
        p_index = n_ind;
    }
    bool solved = true;
    //log_grid(grid_2, n)
    for (int i = 0; solved && i < ((n * n) - 1); i++) {
        if (grid_2[i] != (i + 1)) solved = false;
    }
    return solved;
}