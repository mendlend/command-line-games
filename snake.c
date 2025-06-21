#include "snake.h"

#include <math.h>
#include <ncurses.h>
#include <stdbool.h>
#include <string.h>
#include "utils.h"

#define MAX_LENGTH 100
#define MAX_WIDTH get_width()
#define MAX_HEIGHT get_height()
#define SPEED 64
#define CH_WALL '#'
#define CH_SNAKE '0'
#define CH_FOOD '@'

static void update();
void print_level();
void set_direction(int x, int y);
bool spawn_food();

static bool should_update;
static bool game_end;
static int input;
int score;
int game_end_update;

int snake_x[MAX_LENGTH];
int snake_y[MAX_LENGTH];
int snake_length;
int snake_dir_x;
int snake_dir_y;
bool should_grow;

int food_x;
int food_y;
bool should_spawn_food;

void snake()
{
    // Allow game to start
    should_update = true;
    game_end = false;
    game_end_update = 0;

    // Start curses mode
    initscr();

    // Disable line buffering
    cbreak();

    // Don't echo user input
    noecho();

    // Allow the use of arrow keys
    keypad(stdscr, TRUE);

    // Initialise snake
    snake_length = 1;
    snake_x[0] = MAX_WIDTH / 2;
    snake_y[0] = MAX_HEIGHT / 2;
    snake_dir_x = rand_range(-1, 1);
    if (snake_dir_x == 0)
    {
        do
        {
            snake_dir_y = rand_range(-1, 1);
        } while (snake_dir_y == 0);
    }
    else
    {
        snake_dir_y = 0;
    }
    if (snake_dir_y != 0) { timeout(SPEED * 1.75); }
    else { timeout(SPEED); }
    should_grow = false;
    should_spawn_food = true;
    score = 0;

    while (should_update)
    {
        erase();
        print_level();
        update();
        refresh();
    }

    endwin();
}

static void update()
{
    if (game_end)
    {
        if (game_end_update > 4)
        {
            timeout(-1);
            should_update = false;
        }
        game_end_update++;
        char *msg_game_over = "Game over!";
        char *msg_exit = "Press any key to exit...";
        char *msg_score = "Score: ";
        int score_len = strlen(msg_score) + 3;
        char msg_score_n[score_len];
        snprintf(msg_score_n, score_len, "%s%i", msg_score, score);
        mvprintw(MAX_HEIGHT / 2 - 1, MAX_WIDTH / 2 - (strlen(msg_game_over) - 1) / 2, "%s", msg_game_over);
        mvprintw(MAX_HEIGHT / 2, MAX_WIDTH / 2 - (strlen(msg_score_n) - 1) / 2, "%s", msg_score_n);
        mvprintw(MAX_HEIGHT / 2 + 1, MAX_WIDTH / 2 - (strlen(msg_exit) - 1) / 2, "%s", msg_exit);
        getch();
        return;
    }

    // Get user input
    input = getch();
    switch (input)
    {
        case 'w':
        set_direction(0, -1);
        break;

        case KEY_UP:
        set_direction(0, -1);
        break;

        case 'a':
        set_direction(-1, 0);
        break;

        case KEY_LEFT:
        set_direction(-1, 0);
        break;

        case 's':
        set_direction(0, 1);
        break;

        case KEY_DOWN:
        set_direction(0, 1);
        break;

        case 'd':
        set_direction(1, 0);
        break;

        case KEY_RIGHT:
        set_direction(1, 0);
        break;

        case '0':
        game_end = true;
        break;
    }

    // Move the snake
    if (should_grow && snake_length < MAX_LENGTH)
    {
        snake_x[snake_length] = snake_x[snake_length - 1] + snake_dir_x ;
        snake_y[snake_length] = snake_y[snake_length - 1] + snake_dir_y;
        snake_length++;
        score++;
        should_grow = false;
    }
    else
    {
        for (int i = 0; i < snake_length; i++)
        {
            if (i == snake_length - 1)
            {
                snake_x[i] += snake_dir_x;
                snake_y[i] += snake_dir_y;
            }
            else
            {
                snake_x[i] = snake_x[i + 1];
                snake_y[i] = snake_y[i + 1];
            }
        }
    }

    // Spawn food
    if (should_spawn_food)
    {
        bool good_spawn;
        do
        {
            good_spawn = spawn_food();
        } while (!good_spawn);
        should_spawn_food = false;
    }


    // Check for collision
    if (snake_x[snake_length - 1] < 1 || snake_x[snake_length - 1] >= MAX_WIDTH - 1)
    {
        game_end = true;
    }
    if (snake_y[snake_length - 1] < 1 || snake_y[snake_length - 1] >= MAX_HEIGHT - 1)
    {
        game_end = true;
    }

    // Eat food
    if (snake_x[snake_length - 1] == food_x && snake_y[snake_length - 1] == food_y)
    {
        should_grow = true;
        should_spawn_food = true;
    }

    // Collision with self
    for (int i = 0; i < snake_length - 1; i++)
    {
        if (snake_x[i] == snake_x[snake_length - 1] &&
            snake_y[i] == snake_y[snake_length - 1] && snake_length > 1)
        {
            game_end = true;
        }
    }
}

void print_level()
{
    const int HEIGHT = MAX_HEIGHT;
    const int WIDTH = MAX_WIDTH;
    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x++)
        {
            // Print walls
            if (y == 0 || y == HEIGHT - 1)
            {
                mvaddch(y, x, CH_WALL);
            }
            else if (x == 0 || x == WIDTH - 1)
            {
                mvaddch(y, x, CH_WALL);
            }
        }
    }

    // Print snake
    for (int i = 0; i < snake_length; i++)
    {
        mvaddch(snake_y[i], snake_x[i], CH_SNAKE);
    }

    // Print food
    mvaddch(food_y, food_x, CH_FOOD);
    move(MAX_HEIGHT - 1, MAX_WIDTH - 1);
}

void set_direction(int x, int y)
{
    if (snake_dir_y == -y || snake_dir_x == -x)
    {
        return;
    }
    snake_dir_y = y;
    snake_dir_x = x;
    if (y != 0) { timeout(SPEED * 1.75); }
    else { timeout(SPEED); }
}

bool spawn_food()
{
    food_x = rand_range(1, MAX_WIDTH - 2);
    food_y = rand_range(1, MAX_HEIGHT - 2);
    for (int i = 0; i < snake_length; i++)
    {
        if (snake_x[i] == food_x && snake_y[i] == food_y)
        {
            return false;
        }
    }
    return true;
}
