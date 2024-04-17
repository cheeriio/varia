#ifndef AVL
#define AVL
#include <stdlib.h>

struct _Tree;
typedef struct _Tree Tree;

int cmp(size_t* a, size_t* b);

Tree* insertTree(Tree* tree, size_t* node);

int mem(Tree* tree, size_t* node);

void deleteTree(Tree* tree);

Tree* newTree(size_t* node);

#endif