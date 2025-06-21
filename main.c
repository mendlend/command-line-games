#include <stdio.h>
#include <strings.h>

#include "minesweeper.h"
#include "snake.h"
#include "tictactoe.h"

int main(int argc, char *argv[])
{
    // Check for correct usage
    if (argc != 2)
    {
        printf("Usage: ./play game_name\n\n"
               "Available games:\n"
               "tictactoe   - 2 players, use the keyboard to input the number of a slot\n"
               "snake       - Control using the WASD keys or the arrow keys\n"
               "minesweeper - Right-click or enter coordinates to reveal a tile, middle-click or press Shift + F before entering coordinates to flag a tile\n");
        return 1;
    }

    // Select the specifeid game
    if (strcasecmp(argv[1], "tictactoe") == 0)
    {
        tictactoe();
    }
    else if (strcasecmp(argv[1], "snake") == 0)
    {
        snake();
    }
    else if (strcasecmp(argv[1], "minesweeper") == 0)
    {
        minesweeper();
    }
    else
    {
        printf("Game %s isn't available\n", argv[1]);
        return 1;
    }
    return 0;
}
