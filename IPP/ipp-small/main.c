#include <stdlib.h>
#include <stdio.h>
#include "input.h"
#include "bfs.h"

size_t digits = 0, k = 0;

int main(){
    size_t *dim, *start, *end;
    
    if((dim = inputDim()) == NULL){
        printf("ERROR 1\n");
        return 1;
    }

    if((start = inputNode(dim)) == NULL){
        printf("ERROR 2\n");
        free(dim);
        return 1;
    }
    if((end = inputNode(dim)) == NULL){
        printf("ERROR 2\n");
        free(dim);
        free(start);
        return 1;
    }

    size_t mult = 1, temp;

    for(int i = 0; (size_t)i < k; i++){
        digits += (dim[i] - 1) * mult;
        temp = mult;
        mult *= dim[i];
        if(mult / dim[i] != temp){
            printf("ERROR 0\n");
            free(dim);
            free(start);
            free(end);
            return 1;
        }
    }
    digits++;

    size_t product = 1;
    for(int i = 0; (size_t)i < k; i++)
        product *= dim[i];

    int *code;

    if((code = inputCode(product)) == NULL){
        printf("ERROR 4\n");
        free(dim);
        free(start);
        free(end);
        free(code);
        return 1;
    }

    if(checkWall(dim, start, code) == 1){
        printf("ERROR 1\n");
        free(dim);
        free(start);
        free(end);
        free(code);
        return 1;
    }

    if(checkWall(dim, end, code) == 1){
        printf("ERROR 2\n");
        free(dim);
        free(start);
        free(end);
        free(code);
        return 1;
    }

    Answer* ans = bfs(dim, start, end, code);
    if(ans->exists == 1){
        printf("%ld\n", ans->len);
    }
    else
        printf("NO WAY\n");
    free(ans);
    free(dim);
    free(start);
    free(end);
    free(code);
    return 0;
}