#include "nodefifo.h"

struct _Elem{
    PairND pair;
    Elem* next;
};

struct _Queue{
    Elem* first;
    Elem* last;
    size_t cnt;
};

void insertQueue(Queue* q, size_t* node, size_t depth){
    Elem* new = malloc(sizeof(Elem));
    new->pair.node = node;
    new->pair.depth = depth;
    new->next = NULL;
    if(elems(q) == 0){
        q->first = new;
        q->last = new;
        q->cnt++;
        return;
    }
    q->last->next = new;
    q->last = new;
    q->cnt++;
}

PairND* pop(Queue* q){
    PairND* p = malloc(sizeof(PairND));
    p->node = (q->first)->pair.node;
    p->depth = (q->first)->pair.depth;
    
    Elem* temp = q->first;
    q->first = q->first->next;
    q->cnt--;

    free(temp);
    return p;
}

size_t elems(Queue* q){
    return q->cnt;
}

Queue* newQueue(){
    Queue* new;
    new = malloc(sizeof(Queue));
    new->cnt = 0;
    new->first = NULL;
    new->last = NULL;
    return new;
}

void deleteQueue(Queue* q){
    if(q->cnt == 0){
        free(q);
        return;
    }
    Elem *e = q->first, *temp;
    free(q);
    while(e != NULL){
        temp = e->next;
        free(e);
        e = temp;
    }
    return;
}