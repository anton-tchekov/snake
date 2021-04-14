all:
	gcc snake.c -o snake -Wall -Wextra -O2 -std=c89 -lSDL2

clean:
	rm -f snake
