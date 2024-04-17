#include "bfs.h"
#include <stdio.h>


extern size_t k;

extern size_t digits;

static void genNeighbours(size_t* dim, Tree* t, int* code, Queue* q, size_t* node, size_t depth){
    size_t *p;
    for(size_t i = 0; i < k; i++){
        p = malloc(k * sizeof(size_t));
        for(size_t j = 0; j < k; j++)
            p[j] = node[j];
        if(node[i] - 1 > 0){
            p[i] = node[i] - 1;
            if((checkWall(dim, p, code)) == 0 && (mem(t, p)) == 0){
                insertQueue(q, p, depth);
            }
        }
        else
            free(p);

        p = malloc(k * sizeof(size_t));
        for(size_t j = 0; j < k; j++)
            p[j] = node[j];
        if(node[i] + 1 <= dim[i]){
            p[i] = node[i] + 1;
            if((checkWall(dim, p, code)) == 0 && (mem(t, p)) == 0){
                insertQueue(q, p, depth);
            }
        }
        else
            free(p);
    }
}

static size_t codeBit(size_t* dim, size_t* node){
    size_t bit = 0, mult = 1;
    for(size_t i = 0; i < k; i++){
        bit += mult * (node[i] - 1);
        mult *= dim[i];
    }
    return digits - bit - 1;
}

int checkWall(size_t* dim, size_t* node, int* code){
    size_t i = codeBit(dim, node);
    if(code[i] > 0)
        return 1;
    return 0;
}

Answer* bfs(size_t* dim, size_t* start, size_t* end, int* code){
    Answer* ans = malloc(sizeof(Answer));
    if(cmp(start, end) == 0){
        ans->exists = 1;
        ans->len = 0;
        return ans;
    }
    Queue* q = newQueue();
    Tree* t = newTree(start);
    PairND* p;
    t = insertTree(t, start);
    genNeighbours(dim, t, code, q, start, 1);
    while(elems(q) > 0){
        p = pop(q);
        if(cmp(p->node, end) == 0){

            deleteQueue(q);
            deleteTree(t);
            free(p->node);
            ans->exists = 1;
            ans->len = p->depth;
            return ans;
        }
        t = insertTree(t, p->node);
        genNeighbours(dim, t, code, q, p->node, p->depth + 1);
    }
    ans->exists = 0;
    deleteQueue(q);
    deleteTree(t);

    return ans;
}