all:
	clang -o play main.c minesweeper.c snake.c tictactoe.c utils.c -lncurses -lm
