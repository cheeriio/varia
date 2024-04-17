#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define CODE_MOD 4294967296

extern size_t k;

extern size_t digits;

size_t* inputDim(){
    size_t* dim;
    size_t size = 10;
    size_t *temp = malloc(sizeof(size_t) * size);


    int c, is_num = 0, end = 0;
    size_t cur_num = 0, i = 0, test;

    while(end == 0){
        c = getchar();
        if(c == '\n')
            end = 1;
        if(isdigit(c)){
            is_num = 1;
            test = cur_num;
            cur_num = cur_num * 10;
            if(cur_num / 10 != test){
                free(temp);
                return NULL;
            }
            cur_num += c - '0';
            if(cur_num < 10 * test){
                free(temp);
                return NULL;
            }
        } else if(isspace(c) && is_num == 1){
            is_num = 0;
            if(i == size){
                size_t* new = malloc(sizeof(size_t) * 2 * size);
                size *= 2;
                for(int j = 0; (size_t)j < i; j++)
                    new[j] = temp[j];
                free(temp);
                temp = new;
            }
            if(cur_num < 1){
                free(temp);
                return NULL;
            }
            temp[i] = cur_num;
            i++;
            cur_num = 0;
        } else{
            free(temp);
            return NULL;
        }
    }

    k = i;
    dim = malloc(sizeof(size_t) * k);
    for(int j = 0; (size_t)j < k; j++)
        dim[j] = temp[j];
    free(temp);
    return dim;
}

size_t* inputNode(size_t *dim){
    size_t* p = malloc(k * sizeof(size_t));

    int c = 0, end = 0, is_num = 0;
    size_t cur_num = 0, i = 0, test;

    while(end == 0){
        c = getchar();
        if(c == '\n')
            end = 1;
        if(isdigit(c)){
            is_num = 1;
            if(i == k){ /* too many numbers loaded */
                free(p);
                return NULL;
            }
            test = cur_num;
            cur_num = cur_num * 10;
            if(cur_num / 10 != test){
                free(p);
                return NULL;
            }
            cur_num += c - '0';
            if(cur_num < 10 * test){
                free(p);
                return NULL;
            }
        } else if(isspace(c) && is_num == 1){
            is_num = 0;
            if(cur_num > dim[i]){ /* out of bounds */
                free(p);
                return NULL;
            }
            p[i] = cur_num;
            i++;
            cur_num = 0;
        } else{
            free(p);
            return NULL;
        }
    }
    if(i != k){ /* not enough numbers loaded */
        free(p);
        return NULL;
    }
    return p;
}

int* inputCode(size_t product){
    int c;
    int* code = malloc(digits * sizeof(int));
    if(code == NULL){
        return NULL;
    }

    while(isspace(c = getchar())){}
    if(c == '0'){
        size_t size = digits / 4;
        if(size * 4 != digits)
            size += 1;
        int *temp = malloc(size * sizeof(int)), i = 0;
        c = getchar();
        if(c != 'x' && c != 'X'){
            free(code);
            return NULL;
        }
        int c;
        while((c = getchar()) != '\n'){
            if((size_t)i > size){
                free(code);
                return NULL;
            }
            if(isalpha(c)){
                c = toupper(c);
                if(c > 'F' || c < 'A'){
                    free(code);
                    return NULL;
                }
                temp[i] = c - 'A' + 10;
            } else if(c >= '0' || c <= '9'){
                temp[i] = c - '0';
            } else{
                free(code);
                return NULL;
            }
            i++;
        }

        for(int j = i - 1; j > 0; j--){
            code[digits - 4 * (i - j)] = temp[j] / 8;
            code[digits - 4 * (i - j) + 1] = (temp[j] % 8) / 4;
            code[digits - 4 * (i - j) + 2] = (temp[j] % 4) / 2;
            code[digits - 4 * (i - j) + 3] = temp[j] % 2;
        }
        for(int m = 3; m >= 0; m--){
            if((long long)digits - 4 * i + m < 0 && temp[0] % 2 != 0){
                free(code);
                return NULL;
            }
            code[digits - 4 * i + m] = temp[0] % 2;
            temp[0] /= 2;
        }
        free(temp);
    } else if(c == 'R'){
        size_t nums[5] ={}, test;
        int temp = 0;
        for(int i = 0; i < 5; i++){
            while((isspace(c = getchar()) == 0) || temp == 0){
                if(isspace(c))
                    continue;
                if(!isdigit(c)){
                    free(code);
                    return NULL;
                }
                /* testing for overflow and updating the number */
                test = nums[i];
                nums[i] = nums[i] * 10;
                if(nums[i] / 10 != test){
                    free(code);
                    return NULL;
                }
                nums[i] += c - '0';
                if(nums[i] < 10 * test){
                    free(code);
                    return NULL;
                }
                temp = 1;
            }
        }
        if(nums[2] == 0){
            free(code);
            return NULL;
        }

        for(size_t i = 0; i < digits; i++)
            code[i] = 0;
        
        size_t cur_w, cur_s, iter;
        cur_s = nums[4];
        cur_w = cur_s % product;
        for(size_t i = 1; i <= nums[3]; i++){
            cur_s = (nums[0] * cur_s + nums[1]) % nums[2];
            cur_w = cur_s % product;

            iter = cur_w;
            while(iter < digits){
                code[digits - iter - 1] = 1;
                iter += CODE_MOD;
            }
        }
    } else{
        free(code);
        return NULL;
    }
    return code;
}
