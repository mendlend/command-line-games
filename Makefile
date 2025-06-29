all:
	clang -o /usr/local/bin/play main.c minesweeper.c snake.c tictactoe.c utils.c -lncurses -lm