#ifndef BFS
#define BFS
#include "nodefifo.h"
#include "avl.h"
#include <stdlib.h>

struct _Answer{
    size_t len;
    int exists;
};

typedef struct _Answer Answer;

int checkWall(size_t* dim, size_t* node, int* code);

Answer* bfs(size_t* dim, size_t* start, size_t* end, int* code);

#endif