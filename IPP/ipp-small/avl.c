#include "avl.h"
#include <stdio.h>
extern size_t k;

struct _Tree{
    Tree* left;
    Tree* right;
    size_t* node;
    int height;
};

int cmp(size_t* a, size_t* b){
    for(size_t i = 0; i < k; i++){
        if(a[i] < b[i])
            return 1;
        else if(a[i] > b[i])
            return -1;
    }
    return 0;
}

int mem(Tree* tree, size_t* node){
    if(tree == NULL) /* empty tree */
        return 0;
    int temp = cmp(node, tree->node);
    if(temp == 0)
        return 1;
    else if(temp < 0){
        return mem(tree->left, node);
    }
    return mem(tree->right, node);
}

void deleteTree(Tree* tree){
    if(tree == NULL)
        return;
    if(tree->left != NULL)
        deleteTree(tree->left);
    if(tree->right != NULL)
        deleteTree(tree->right);
    free(tree);
} 

static size_t max(size_t a, size_t b)
{
    return (a > b)? a : b;
}

static size_t height(Tree *t)
{
    if (t == NULL)
        return 0;
    return 1+max(height(t->left), height(t->right));
}


Tree* newTree(size_t* node)
{
    Tree* t = malloc(sizeof(Tree));
    t->node = node;
    t->left = NULL;
    t->right = NULL;
    t->height = 0;
    return(t);
}

static Tree* rightRotate(Tree *y)
{
    Tree* x = y->left;
    Tree* T2 = x->right;
 
    x->right = y;
    y->left = T2;
 
    y->height = height(y);
    x->height = height(x);
 
    return x;
}

static Tree* leftRotate(Tree* x)
{
    Tree* y = x->right;
    Tree* T2 = y->left;
 
    y->left = x;
    x->right = T2;
 
    x->height = height(x);
    y->height = height(y);

    return y;
}

static size_t getBalance(Tree* t)
{
    if (t == NULL)
        return 0;
    return height(t->left) - height(t->right);
}

Tree* insertTree(Tree* t, size_t* node)
{
    if (t == NULL)
        return(newTree(node));
    int temp1 = cmp(node, t->node), temp2;
    if (temp1 < 0){
        t->left  = insertTree(t->left, node);
    }
    else if (temp1 > 0){
        t->right = insertTree(t->right, node);
    }
    else
        return t;
    t->height = height(t);
    int balance = getBalance(t);
    if(balance > 1)
        temp1 = cmp(node, t->left->node);
    if(balance < -1)
        temp2 = cmp(node, t->right->node);

    if (balance > 1 && temp1 < 0)
        return rightRotate(t);

    if (balance < -1 && temp2 > 0)
        return leftRotate(t);

    if (balance > 1 && temp1 > 0)
    {
        t->left =  leftRotate(t->left);
        return rightRotate(t);
    }

    if (balance < -1 && temp2 < 0)
    {
        t->right = rightRotate(t->right);
        return leftRotate(t);
    }
    return t;
}