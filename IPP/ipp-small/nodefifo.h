#ifndef NODEFIFO
#define NODEFIFO
#include <stdlib.h>
struct _Elem;
typedef struct _Elem Elem;

struct _Queue;
typedef struct _Queue Queue;

struct _PairND{
    size_t* node;
    size_t depth;
};
typedef struct _PairND PairND;

void insertQueue(Queue* q, size_t* node, size_t depth);

PairND* pop(Queue* q);

size_t elems(Queue* q);

Queue* newQueue();

void deleteQueue(Queue* q);

#endif