#include "utils.h"

#include <ncurses.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

bool is_seeded = false;

// Move down or up relative to the current cursor position
void move_rel_y(int amount)
{
    int y, x;
    getyx(stdscr, y, x);
    move(y + amount, x);
}

// Move only the x coordinate of the cursor
void move_x(int new_x)
{
    int y, x;
    getyx(stdscr, y, x);
    move(y, new_x);
}

// Move the cursor to the beginning and down by 'lines'
void new_line(int lines)
{
    if (lines < 1)
    {
        return;
    }
    else
    {
        move_x(0);
        move_rel_y(lines);
    }
}

// Returns the width of the window
int get_width()
{
    int y, x;
    getmaxyx(stdscr, y, x);
    return x;
}

// Returns the height of the window
int get_height()
{
    int y, x;
    getmaxyx(stdscr, y, x);
    return y;
}

// Returns a random number between the specified range, automatically seeded with the current time
int rand_range(int min, int max)
{
    if (!is_seeded)
    {
        srand(time(0));
        is_seeded = true;
    }
    return rand() % (max - min + 1) + min;
}
