all:
	gcc -I src/include -L src/lib -o clock main.c -Wall -Wextra -lmingw32 -lSDL2main -lSDL2