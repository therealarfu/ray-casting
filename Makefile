all:
	gcc main.c -o raycasting -lmingw32 -lSDL2main -lSDL2

run: all
	./raycasting