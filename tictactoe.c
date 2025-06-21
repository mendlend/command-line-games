#include "tictactoe.h"

#include <ncurses.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"

#define GRID_LEN 3
#define GRID_SIZE GRID_LEN * GRID_LEN
#define GRID_P1 1
#define GRID_P2 -1
#define GRID_EMPTY 0
#define CH_P1 'O'
#define CH_P2 'X'
#define MSG_SIZE 16

static void update();
static void print_grid();

void check_winner();
int check_horizontal();
int check_vertical();
int check_diagonal_lr();
int check_diagonal_rl();

// The game grid
static int grid[GRID_SIZE] = {GRID_EMPTY};

static bool should_update;
static bool game_end;
static int input;
int current_player;
int winner;
static char message[MSG_SIZE];

void tictactoe()
{
    // Allow game to start
    should_update = true;
    game_end = false;

    // Start curses mode
    initscr();

    // Disable line buffering
    cbreak();

    // Don't echo user input
    noecho();

    // Set up colours
    start_color();
    use_default_colors();
    init_pair(1, COLOR_CYAN, -1);
    init_pair(2, COLOR_RED, -1);

    current_player = 1;

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
    // If the game has ended, exit game on any button
    if (game_end)
    {
        should_update = false;
    }

    // Get user input
    input = getch();
    input -= '0';
    message[0] = '\0';

    // If input is 0, exit the game
    if (input == 0)
    {
        should_update = false;
        return;
    }

    // Check for valid input
    else if (input < 1 || input > GRID_SIZE)
    {
        snprintf(message, MSG_SIZE, "Invalid number");
    }

    // Update grid
    else
    {
        // Is grid spot already taken?
        if (grid[input - 1] != 0)
        {
            snprintf(message, MSG_SIZE, "Already taken!");
        }
        else
        {
            grid[input - 1] = current_player;
            current_player *= -1;
        }
    }

    // Check for a winner
    check_winner();
}

static void print_grid()
{
    // Reset cursor position
    move(0, 0);

    // Print grid
    printw("     |     |    ");
    new_line(1);
    for (int i = 0; i < GRID_SIZE; i++)
    {
        if (grid[i] == GRID_P1)
        {
            attron(COLOR_PAIR(1));
            printw("  %c  ", CH_P1);
            attroff(COLOR_PAIR(1));
        }
        else if (grid[i] == GRID_P2)
        {
            attron(COLOR_PAIR(2));
            printw("  %c  ", CH_P2);
            attroff(COLOR_PAIR(2));
        }
        else
        {
            printw("  %i  ", i + 1);
        }

        // Go to new line every 3 grid slots
        if ((i + 1) % GRID_LEN == 0)
        {
            new_line(1);
            if (i > GRID_SIZE - GRID_LEN)
            {
                printw("     |     |    ");
            }
            else
            {
                printw("_____|_____|_____");
                new_line(1);
                printw("     |     |    ");
            }
            new_line(1);
        }

        // Print vertical lines
        else
        {
            printw("|");
        }
    }

    // Print messages
    if (game_end)
    {
        new_line(1);
        if (winner == 0)
        {
            printw("Tie!");
        }
        else
        {
            printw("Player %i won!", winner);
        }
        new_line(1);
        printw("Press any key to exit...");
    }
    else
    {
        new_line(1);
        int p = current_player;
        if (p != 1) { p = 2; }
        printw("Player %i's turn", p);
        new_line(1);
        printw("%s", message);
    }
}

void check_winner()
{
    winner = check_horizontal();
    if (abs(winner) != GRID_LEN)
    {
        winner = check_vertical();
        if (abs(winner) != GRID_LEN)
        {
            winner = check_diagonal_lr();
            if (abs(winner) != GRID_LEN)
            {
                winner = check_diagonal_rl();
            }
        }
    }

    // Decode which player won
    if (winner == GRID_LEN)
    {
        winner = 1;
        game_end = true;
    }
    else if (winner == -GRID_LEN)
    {
        winner = 2;
        game_end = true;
    }

    // If there is no winner, check if there are any remaining grid slots
    else
    {
        game_end = true;
        winner = 0;
        for (int i = 0; i < GRID_SIZE; i++)
        {
            if (grid[i] == 0)
            {
                game_end = false;
            }
        }
    }
}

// Check for a winner in the horizontal direction
int check_horizontal()
{
    // Add the first 3 elements of the grid, if there is no winner,
    // add the next 3 elements, and so on
    for (int i = 0; i < GRID_SIZE; i += GRID_LEN)
    {
        int sum = grid[i] + grid[i + 1] + grid[i + 2];
        if (abs(sum) == GRID_LEN) { return sum; }
    }
    return 0;
}

// Check for a winner in the vertical direction
int check_vertical()
{
    // Add the every 3 elements of the grid, if there is no winner,
    // add the next 3 elements, and so on
    for (int i = 0; i < GRID_LEN; i++)
    {
        int sum = grid[i] + grid[i + 3] + grid[i + 6];
        if (abs(sum) == GRID_LEN) { return sum; }
    }
    return 0;
}

// Check for a winner in the top-left -> bottom-right diagonal direction
int check_diagonal_lr()
{
    // Element 0, 4 and 8 of the grid line up diagonally from left to right
    return grid[0] + grid[4] + grid[8];
}

// Check for a winner in the top-right -> bottom-left diagonal direction
int check_diagonal_rl()
{
    // Element 2, 4 and 6 of the grid line up diagonally from right to left
    return grid[2] + grid[4] + grid[6];
}
