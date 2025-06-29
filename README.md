# Command Line Games
#### Video Demo:  <URL HERE>
## Description
This project is a terminal-based game suite written in C, featuring Minesweeper, Snake, and Tic Tac Toe. The games use ASCII graphics and are rendered using the ncurses library. To play, run the following command:
```bash
play game_name
```
### What I used
The project is written in C and uses simple ASCII graphics with the help of the ncurses library. The ncurses library makes it easier to render the games by giving increased access to the terminal, allowing the printing of characters anywhere on the screen and changing their colours. It also gives more control when handling input, like not echoing inputted characters and disabling line buffering, which is utilised in all of the games. It also allows the snake game to update without waiting for the user's input, and finally, it doesn't clutter up the terminal after a game ends, allowing users to continue right where they left off.
## Games
### Tic Tac Toe
The first game I implemented was a simple two-player game of Tic Tac Toe. The grid is printed onto the screen with numbers assigned to each slot. The players take turns to press a number on the keyboard to select that slot, whoever gets three in a row wins, if no one gets three in a row and there are no empty slots available, the game ends as a tie.

`tictactoe.c` implements the full logic for Tic Tac Toe. It includes:
- Initialization of variables and the ncurses environment
- A main game loop that clears the screen, prints the grid, handles input, and checks for win conditions
- Player input via number keys (1-9) mapped to board positions
- Player values (1 and -1) are used to simplify turn switching and win detection (e.g., three of the same value add up to 3 or -3)
- Grid display with color-coded X and O, updated each frame
### Snake
The second game I implemented is a classic ASCII version of Snake. The snake is controlled using either the arrow keys or the WASD keys. If the snake collides with itself or a wall, the game ends. The player can eat food spawned at a random location to grow the snake and increase the score.

`snake.c` contains the complete logic for the Snake game, including input handling, snake movement, food spawning, collision detection, and rendering. Here’s a breakdown of the key functionality:
- The snake starts in the centre of the screen with a random initial direction.
- The initial length is 1, and the score is set to 0.
- The game runs in a loop that erases the screen, prints the game state (`print_level()`), calls `update()`, and refreshes the display.
- Direction adjusts the speed slightly so vertical movement is slower for balance.
- The snake moves forward by shifting its body segments and updating the head position.
- If food is eaten, the snake grows by one segment and the score increases.
- Randomly places food on the grid, avoiding any tiles currently occupied by the snake.
- The game ends if the snake runs into the wall or collides with itself
- After a short delay, a "Game Over" screen is shown with the player's final score. The user can press any key to exit.
### Minesweeper
The last game I implemented is a 9x9 version of Minesweeper with 10 randomly placed mines. The objective is to reveal all non-mine tiles without detonating a mine. Minesweeper supports both keyboard and mouse controls.

You can reveal a tile either by clicking the left mouse button or by entering coordinates using the keyboard (e.g., pressing `a` followed by `1` to select the top-left tile). If a revealed tile has no adjacent mines, the game automatically reveals all surrounding safe tiles using a recursive flood-fill function.

To help avoid mines, you can place flags on suspected mine tiles. Flags can be toggled using the middle mouse button or by pressing `Shift + F`, then entering the tile's coordinates. Right-clicking is not supported due to terminal paste behaviour.

The game ends immediately if you reveal a mine, showing all mine locations and flag placements. If all safe tiles are revealed without triggering a mine, you win. A live timer and flag counter are displayed throughout the game. The interface uses colours to distinguish numbers, flags, and mines for clarity.

`minesweeper.c` implements the full logic for Minesweeper. It includes:
- Initializes a 9x9 grid of tiles and places 10 unique mines randomly using `rand_range()`
- Uses two parallel arrays: one for tile states (opened/unopened), and one for tile values (number of adjacent mines)
- `print_grid()` draws the entire grid with coloured tile values, unopened tiles (`#`), flags (`F`), and mines (`@`)
- Coordinates (letters for columns, numbers for rows) are printed beside the grid for keyboard input
- `reveal_tile()` opens a tile and ends the game if it's a mine
- `reveal()` uses flood-fill to automatically open adjacent empty tiles
- `find_adjacent_mines()` calculates the number of neighbouring mines for each tile
- `add_flag()` adds/removes a flag and ensures no duplicates
- `is_mine()` and `is_flag()` are utility functions for checking tile status
## Other Files:
### main.c
This file handles program startup and game selection.
- Checks if the correct number of command-line arguments is provided
- If not, it displays usage instructions and exits
- If a valid game name is entered (case-insensitive), it launches the corresponding game
- If the input is invalid, it displays an error and exits
### utils.c
This file contains utility functions used by multiple games, especially for cursor control:
- `move_rel_y(n)`: Moves the cursor vertically by `n` rows
- `move_x(x)`: Moves the cursor horizontally to column `x`
- `new_line(n)`: Moves the cursor down `n` lines, starting at column 0
- `get_width()`, `get_height()`: Return current terminal dimensions
- `rand_range(min, max)`: Returns a random number in the given range. Seeds `rand()` using the current time if not already seeded
