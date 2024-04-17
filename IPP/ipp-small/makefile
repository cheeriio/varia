CC       = gcc
CFLAGS   = -Wall -Wextra -Wno-implicit-fallthrough -std=c17 -O2 -g -s

.PHONY: all clean

all: labyrinth

labyrinth: main.o input.o bfs.o avl.o nodefifo.o
	$(CC) $(CFLAGS) main.o input.o bfs.o avl.o nodefifo.o -o labyrinth
nodefifo.o: nodefifo.h nodefifo.c
	$(CC) $(CFLAGS) -c nodefifo.c -o nodefifo.o
input.o: input.h input.c
	$(CC) $(CFLAGS) -c input.c -o input.o
main.o: input.h main.c
	$(CC) $(CFLAGS) -c main.c -o main.o
avl.o: avl.h avl.c
	$(CC) $(CFLAGS) -c avl.c -o avl.o
bfs.o: bfs.h bfs.c
	$(CC) $(CFLAGS) -c bfs.c -o bfs.o
clean:
	rm -f *.o labyrinth
