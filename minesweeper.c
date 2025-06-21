#include "minesweeper.h"

#include <math.h>
#include <ncurses.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "utils.h"

#define GRID_LEN 9
#define GRID_SIZE GRID_LEN * GRID_LEN
#define GRID_OPENED 0
#define GRID_UNOPENED 1
#define MAX_MINES 10
#define CH_GRID_UNOPENED '#'
#define CH_GRID_EMPTY '.'
#define CH_GRID_FLAGGED 'F'
#define CH_GRID_MINE '@'
#define C_BLUE 2
#define C_GREEN 3
#define C_RED 1
#define C_MAGENTA 4
#define C_CYAN 5
#define C_YELLOW 6
#define MSG_SIZE 64
#define ADJACENT_OFFSET_N 8
const int ADJACENT_OFFSETS[ADJACENT_OFFSET_N] = { -GRID_LEN - 1, -GRID_LEN, -GRID_LEN + 1, -1, 1, GRID_LEN - 1, GRID_LEN, GRID_LEN + 1 };

static void update();
static void print_grid();
int find_adjacent_mines(int grid_index);
void reveal(int grid_index);
bool is_mine(int grid_index);
void reveal_tile(int x, int y);
bool is_flag(int grid_index, int *flag_index);
void add_flag(int grid_index);
int i_to_x(int grid_index);
int i_to_y(int grid_index);
void reset_input();

static bool should_update;
static bool game_end;
static int input;
int input_x;
int input_y;
bool should_flag;
bool game_won;
MEVENT event;
int time_start;
int time_elapsed;
bool time_update;

static int grid[GRID_SIZE]; // Store tile state (opened or unopened)
int tiles[GRID_SIZE] = {0}; // Store adjacent mine numbers
int mines[MAX_MINES] = {0}; // Store mine locations
int flags[GRID_SIZE] = {0}; // Store flag locations
int flags_amount;
static char message[MSG_SIZE];

void minesweeper()
{
    // Allow game to start
    should_update = true;
    game_end = false;
    game_won = false;
    should_flag = false;
    reset_input();

    // Start curses mode
    initscr();

    // Disable line buffering
    cbreak();

    // Don't echo user input
    noecho();

    // Set up colours
    start_color();
    use_default_colors();
    init_pair(C_BLUE, COLOR_BLUE, -1);
    init_pair(C_GREEN, COLOR_GREEN, -1);
    init_pair(C_RED, COLOR_RED, -1);
    init_pair(C_MAGENTA, COLOR_MAGENTA, -1);
    init_pair(C_CYAN, COLOR_CYAN, -1);
    init_pair(C_YELLOW, COLOR_YELLOW, -1);

    // Set up mouse input
    keypad(stdscr, TRUE);
    mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION, NULL);

    // Set grid to unopened
    for (int i = 0; i < GRID_SIZE; i++)
    {
        grid[i] = GRID_UNOPENED;
    }

    // Rig mines
    int placed_mines = 0;
    bool good_pos;
    do
    {
        good_pos = true;
        int pos = rand_range(0, GRID_SIZE - 1);
        for (int i = 0; i < placed_mines; i++)
        {
            if (mines[i] == pos)
            {
                good_pos = false;
                break;
            }
        }
        if (good_pos)
        {
            mines[placed_mines] = pos;
            placed_mines++;
        }
    } while (placed_mines < MAX_MINES);

    // Assign a number af adjacent mines to each tile
    for (int i = 0; i < GRID_SIZE; i++)
    {
        tiles[i] = find_adjacent_mines(i);
    }

    flags_amount = 0;

    // Start timer
    time_start = time(NULL);
    nodelay(stdscr, true);
    time_update = true;

    while (should_update)
    {
        erase();
        print_grid();
        update();
        refresh();
    }

    endwin();
}

static void update()
{
    // Track time
    if (time_update)
    {
        time_elapsed = time(NULL) - time_start;
    }

    // Stop timer and pause game after game ends
    if (game_end)
    {
        time_update = false;
        nodelay(stdscr, false);
    }

    // Get user input
    input = getch();

    if (game_end)
    {
        if (input != KEY_MOUSE)
        {
            should_update = false;
            return;
        }
        return;
    }

    // Mouse input
    if (input == KEY_MOUSE)
    {
        if (getmouse(&event) == OK)
        {
            // Left mouse button: reveal tile
            if (event.bstate & BUTTON1_CLICKED)
            {
                if (event.y < GRID_LEN && event.x < GRID_LEN * 2)
                {
                    reveal_tile((event.x - 1) / 2, event.y);
                }
            }

            // Middle mouse button: flag a tile
            // NOTE: right mouse button doesn't always work because the terminal uses it for pasting from the clipboard
            else if (event.bstate & BUTTON2_CLICKED)
            {
                if (event.y < GRID_LEN && event.x < GRID_LEN * 2)
                {
                    int grid_index = event.y * GRID_LEN + (event.x - 1) / 2;
                    if (grid[grid_index] == GRID_UNOPENED)
                    {
                        add_flag(grid_index);
                    }
                }
            }
        }
    }

    // Keyboard input
    else if (input == '0')
    {
        should_update = false;
    }
    else if (input >= 'a' && input <= 'i')
    {
        input_x = input - 'a';
    }
    else if (input >= '1' && input <= '9')
    {
        input_y = input - '1';
        if (input_x >= 0)
        {
            if (should_flag)
            {
                add_flag(input_y * GRID_LEN + input_x);
                should_flag = false;
            }
            else
            {
                reveal_tile(input_x, input_y);
            }
            reset_input();
        }
    }
    else if (input == 'F')
    {
        should_flag = !should_flag;
    }
    else if (input == KEY_BACKSPACE)
    {
        reset_input();
    }

    // Check for win
    bool win = true;
    for (int i = 0; i < GRID_SIZE; i++)
    {
        if (grid[i] == GRID_UNOPENED && !is_mine(i))
        {
            win = false;
            break;
        }
    }
    if (win)
    {
        game_won = true;
        game_end = true;
        snprintf(message, MSG_SIZE, "You won!");
    }
}

static void print_grid()
{
    for (int y = 0; y < GRID_LEN; y++)
    {
        move(y, 0);
        for (int x = 0; x < GRID_LEN; x++)
        {
            // Ensure everyting is evenly spaced out
            addch(' ');

            // Convert x and y coordinates to a grid index (y is already a multiple of 9)
            int grid_index = y * GRID_LEN + x;

            // Print tiles
            if (grid[grid_index] == GRID_OPENED)
            {
                // If there are adjacent mines, print a number
                if (tiles[grid_index] > 0)
                {
                    attron(COLOR_PAIR(tiles[grid_index] % 3 + 1));
                    addch(tiles[grid_index] + '0');
                    attroff(COLOR_PAIR(tiles[grid_index] % 3 + 1));
                }
                else
                {
                    addch(CH_GRID_EMPTY);
                }
            }
            else
            {
                addch(CH_GRID_UNOPENED);
            }
        }
    }

    // Print coordinates
    for (int y = 0; y < GRID_LEN; y++)
    {
        if (y == input_y)
        {
            attron(COLOR_PAIR(C_CYAN));
        }
        mvaddch(y, GRID_LEN * 2 + 2, y + 1 + '0');
        attroff(COLOR_PAIR(C_CYAN));
    }
    for (int x = 0; x < GRID_LEN; x++)
    {
        if (x == input_x)
        {
            attron(COLOR_PAIR(C_CYAN));
        }
        mvaddch(GRID_LEN + 1, x * 2 + 1, x + 'a');
        attroff(COLOR_PAIR(C_CYAN));
    }

    // Reset cursor position
    move(0, 0);

    if (game_end)
    {
        // Reveal all mines if the game has ended
        move(0, 0);
        for (int i = 0; i < MAX_MINES; i++)
        {
            mvaddch(i_to_y(mines[i]), i_to_x(mines[i]) * 2 + 1, CH_GRID_MINE);
        }
    }

    // Print flags
    for (int i = 0; i < flags_amount; i++)
    {
        if (game_end && is_mine(flags[i]))
        {
            attron(COLOR_PAIR(C_GREEN));
        }
        else
        {
            attron(COLOR_PAIR(C_RED));
        }
        attron(A_BOLD);
        mvaddch(i_to_y(flags[i]), i_to_x(flags[i]) * 2 + 1, CH_GRID_FLAGGED);
        attroff(A_BOLD | COLOR_PAIR(C_RED) | COLOR_PAIR(C_GREEN));
    }

    // Calculate indicator position
    int indicators_x = GRID_LEN * 2 + 4;

    // Print timer
    move(0, indicators_x);
    printw("Time: %i", time_elapsed);

    // Flag count
    move_x(indicators_x);
    move_rel_y(1);
    printw("Flags: %i/%i", flags_amount, MAX_MINES);

    // Flag indicator
    if (should_flag)
    {
        move_x(indicators_x);
        move_rel_y(1);
        attron(A_BOLD | COLOR_PAIR(C_RED));
        printw("Flag");
        attroff(A_BOLD | COLOR_PAIR(C_RED));
    }

    // Print any messages
    move(GRID_LEN + 3, 0);
    printw("%s", message);
    if (game_end)
    {
        new_line(1);
        printw("Press any key to exit...");
    }
}

// Find all mines in the adjacent 8 tiles to the given grid index
int find_adjacent_mines(int grid_index)
{
    int x = i_to_x(grid_index);
    int y = i_to_y(grid_index);

    int adjacent_mines = 0;
    for (int i = 0; i < ADJACENT_OFFSET_N; i++)
    {
        int adjacent_index = grid_index + ADJACENT_OFFSETS[i];
        if (adjacent_index >= 0 && adjacent_index < GRID_SIZE)
        {
            // If a tile is on the edge, make sure tiles from the other side of the grid don't count
            int a_x = i_to_x(adjacent_index);
            int a_y = i_to_y(adjacent_index);
            if(abs(a_x - x) > 1 || abs(a_y - y) > 1)
            {
                continue;
            }

            if (is_mine(adjacent_index))
            {
                adjacent_mines++;
            }
        }
    }
    return adjacent_mines;
}

// Reveal tiles and its adjacent ones if there are no adjacent mines (flood fill)
void reveal(int grid_index)
{
    if (grid_index < 0 || grid_index >= GRID_SIZE)
    {
        return;
    }
    if (grid[grid_index] == GRID_OPENED)
    {
        return;
    }

    grid[grid_index] = GRID_OPENED;

    if (tiles[grid_index] > 0)
    {
        return;
    }

    int x = i_to_x(grid_index);
    int y = i_to_y(grid_index);

    for (int i = 0; i < ADJACENT_OFFSET_N; i++)
    {
        int adjacent_index = grid_index + ADJACENT_OFFSETS[i];
        if (adjacent_index >= 0 && adjacent_index < GRID_SIZE)
        {
            // If a tile is on the edge, make sure tiles from the other side of the grid don't count
            int a_x = i_to_x(adjacent_index);
            int a_y = i_to_y(adjacent_index);
            if(abs(a_x - x) > 1 || abs(a_y - y) > 1)
            {
                continue;
            }

            reveal(adjacent_index);
        }
    }
}

// Check if there is a mine at the given index
bool is_mine(int grid_index)
{
    for (int i = 0; i < MAX_MINES; i++)
    {
        if (mines[i] == grid_index)
        {
            return true;
        }
    }
    return false;
}

// Reveal the specified tile or end game if there is a mine
void reveal_tile(int x, int y)
{
    int grid_index = y * GRID_LEN + x;
    if (grid[grid_index] == GRID_UNOPENED)
    {
        if (is_flag(grid_index, NULL))
        {
            return;
        }
        if (is_mine(grid_index))
        {
            game_end = true;
            snprintf(message, MSG_SIZE, "You lost :(");
        }
        else
        {
            reveal(grid_index);
        }
    }
}

bool is_flag(int grid_index, int *flag_index)
{
    for (int i = 0; i < flags_amount; i++)
    {
        if (flags[i] == grid_index)
        {
            if (flag_index != NULL)
            {
                *flag_index = i;
            }
            return true;
        }
    }
    return false;
}

// Add or remove a flag at the specified grid index
void add_flag(int grid_index)
{
    // Check if there is already a flag at that position
    int flag_index;
    bool is_flagged = is_flag(grid_index, &flag_index);

    // If there is a flag remove it
    if (is_flagged)
    {
        // Shift the rest of the array to the right
        for (int j = flag_index; j < flags_amount - 1; j++)
        {
            flags[j] = flags[j + 1];
        }
        flags_amount--;
    }

    // Otherwise, add a flag
    else
    {
        flags[flags_amount] = grid_index;
        flags_amount++;
    }
}

// Convert a grid index to its x coordinate (additional calcutations are necessary before printing to the screen, to account for white spaces between characters)
int i_to_x(int grid_index)
{
    return grid_index % GRID_LEN;
}

// Convert a grid index to its y coordinate
int i_to_y(int grid_index)
{
    return grid_index / GRID_LEN;
}

void reset_input()
{
    input_x = -1;
    input_y = -1;
}
