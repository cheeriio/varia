/** @file
 * Implementacja klasy przechowującej przekierowania numerów telefonicznych
 *
 * @author Michał Płachta <mp438675@students.mimuw.edu.pl>
 * @copyright Uniwersytet Warszawski
 * @date 2022
 */


#include "phone_forward.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>

static const int DIGITS = 12;

static const char digits[12] = "0123456789*#";

static int isPhDigit(char c){
    for(int i = 0; i < DIGITS; i++){
        if(digits[i] == c)
            return 1;
    }
    return 0;
}

static int getIndex(char c){
    for(int i = 0 ; i < DIGITS; i++){
        if(digits[i] == c)
            return i;
    }
    return -1;
}

typedef struct _PhoneFwdTree{
    struct _PhoneFwdTree** lower;   /**< Lista połączeń zawierających kolejna cyfrę (0 - #). */
    char* fwd;  /**< Numer na który przekierowuje numer reprezentowany przez strukturę. */
} PhoneFwdTree;

typedef struct _PhoneRevTree{
    struct _PhoneRevTree** lower;   /**< Lista połączeń zawierających kolejna cyfrę (0 - #). */
    PhoneNumbers* fwds; /**<  Lista numerów które przekierowują na ten numer. */
} PhoneRevTree;

struct PhoneForward{
    PhoneFwdTree* treeFwd;  /**<  Drzewo przechowujace przekierowania na inne numery. */
    PhoneRevTree* treeRev;  /**<  Drzewo przechowujace numery przeierowujace na dany wierzcholek */
};

struct PhoneNumbers{
    char** nums;    /**< Tablica numerów. */
    int cnt;    /**< Licznik numerów. */
    int size;   /**< Długość tablicy numerów. */
};

PhoneNumbers* phnumNew(){
    PhoneNumbers* p = malloc(sizeof(PhoneNumbers));
    if(p == NULL)
        return NULL;
    p->nums = malloc(10 * sizeof(char*));
    if(p->nums == NULL){
        free(p);
        return NULL;
    }
    p->cnt = 0;
    p->size = 10;
    return p;
}

int phnumSize(PhoneNumbers* pnum){
    return pnum->cnt;
}

void phnumAdd(PhoneNumbers* pnums, char const* newNum){
    if(newNum != NULL){
        for(size_t i = 0; i < strlen(newNum); i++)
            if(!isPhDigit(newNum[i]))
                return;
    }
    if(phnumContains(pnums, newNum) == 1)
        return;
    if(pnums->size == pnums->cnt){
        char** temp = malloc(2 * pnums->size * sizeof(char*));
        for(int i = 0; i < pnums->cnt; i++)
            temp[i] = pnums->nums[i];
        free(pnums->nums);
        pnums->nums = temp;
        pnums->size *= 2;
    }
    if(newNum == NULL){
        pnums->nums[pnums->cnt] = NULL;
        pnums->cnt++;
        return;
    }
    pnums->nums[pnums->cnt] = malloc((strlen(newNum) + 1) * sizeof(char));
    strcpy(pnums->nums[pnums->cnt], newNum);
    pnums->cnt++;
}

void phnumDelete(PhoneNumbers *pnum){
    if(pnum == NULL)
        return;
    for(int i = 0; i < pnum->cnt; i++)
        free(pnum->nums[i]);
    free(pnum->nums);
    free(pnum);
}

char const * phnumGet(PhoneNumbers const *pnum, size_t idx){
    if(pnum == NULL)
        return NULL;
    if((size_t)pnum->cnt <= idx)
        return NULL;
    return pnum->nums[idx];
}

int phnumContains(PhoneNumbers* pnum, char const* number){
    for(int i = 0; i < pnum->cnt; i++){
        if(number == NULL){
            if(pnum->nums[i] == NULL)
                return 1;
        }
        else if(pnum->nums[i] != NULL && strcmp(number, pnum->nums[i]) == 0){
            return 1;
        }
    }
    return 0;
}

void phnumRemove(PhoneNumbers* pnum, char const* number){
    int i = 0;
    for(; i < pnum->cnt; i++){
        if(number == NULL){
            if(pnum->nums[i] == NULL)
                break;
        }
        else if(pnum->nums[i] != NULL && strcmp(number, pnum->nums[i]) == 0){
            free(pnum->nums[i]);
            break;
        }
    }
    for(int j = i + 1; j < pnum->cnt; j++){
        pnum->nums[j-1] = pnum->nums[j];
    }
    pnum->cnt -= 1;
}

static int numComp(char const** num1, char const** num2){
    int a = strlen(*num1);
    int b = strlen(*num2);
    int x = a;
    if(b < a)
        x = b;
    for(int i = 0; i < x; i++){
        int cmp = getIndex((*num1)[i]) - getIndex((*num2)[i]);
        if(cmp < 0)
            return -1;
        if(cmp > 0)
            return 1;
    }
    if(a == b)
        return 0;
    if(a > b)
        return 1;
    return -1;
}

void phnumSort(PhoneNumbers* pnum){
    qsort(pnum->nums, pnum->cnt, sizeof(char*), (int (*)(const void*, const void*)) numComp); // By uniknąć ostrzeżenia.
}

PhoneFwdTree* phfwdTreeNew(){
    PhoneFwdTree* p = malloc(sizeof(PhoneFwdTree));
    if(p == NULL)
        return NULL;
    p->fwd = NULL;
    p->lower = malloc(DIGITS * sizeof(PhoneFwdTree*));
    if(p->lower == NULL){
        free(p);
        return NULL;
    }
    for(int i = 0; i < DIGITS; i++){
        p->lower[i] = NULL;
    }
    return p;
}

PhoneRevTree* phrevTreeNew(){
    PhoneRevTree* p = malloc(sizeof(PhoneRevTree));
    if(p == NULL)
        return NULL;
    p->fwds = phnumNew();
    if(p->fwds == NULL){
        free(p);
        return NULL;
    }
    p->lower = malloc(DIGITS * sizeof(PhoneRevTree*));
    if(p->lower == NULL){
        free(p);
        phnumDelete(p->fwds);
        return NULL;
    }
    for(int i = 0; i < DIGITS; i++){
        p->lower[i] = NULL;
    }
    return p;
}

PhoneForward* phfwdNew(void){
    PhoneForward* p = malloc(sizeof(PhoneForward));
    if(p == NULL)
        return NULL;
    p->treeFwd = malloc(sizeof(PhoneFwdTree));
    if(p->treeFwd == NULL){
        free(p);
        return NULL;
    }
    p->treeRev = malloc(sizeof(PhoneRevTree));
    if(p->treeRev == NULL){
        free(p->treeFwd);
        free(p);
        return NULL;
    }
    p->treeFwd->fwd = NULL;
    p->treeFwd->lower = malloc(DIGITS * sizeof(PhoneFwdTree*));
    if(p->treeFwd->lower == NULL){
        free(p->treeFwd);
        free(p->treeRev);    
        free(p);    
        return NULL;
    }
    p->treeRev->lower = malloc(DIGITS * sizeof(PhoneRevTree*));
    if(p->treeRev->lower == NULL){
        free(p->treeFwd);
        free(p->treeRev);
        free(p->treeFwd->lower);
        free(p);        
        return NULL;
    }
    for(int i = 0; i < DIGITS; i++){
        p->treeFwd->lower[i] = NULL;
        p->treeRev->lower[i] = NULL; 
    }
    p->treeRev->fwds = phnumNew();
    return p;
}

static void phfwdTreeDelete(PhoneFwdTree* t){
    if(t == NULL)
        return;
    for(int i = 0; i < DIGITS; i++){
        if(t->lower[i] != NULL)
            phfwdTreeDelete(t->lower[i]);
    }
    free(t->fwd);
    free(t->lower);
    free(t);
}

static void phrevTreeDelete(PhoneRevTree* t){
    if(t == NULL)
        return;
    for(int i = 0; i < DIGITS; i++){
        if(t->lower[i] != NULL)
            phrevTreeDelete(t->lower[i]);
    }
    phnumDelete(t->fwds);
    free(t->lower);
    free(t);
}

void phfwdDelete(PhoneForward *pf){
    if(pf == NULL)
        return;
    phfwdTreeDelete(pf->treeFwd);
    phrevTreeDelete(pf->treeRev);
    free(pf);
}

static bool phfwdRecAddRev(PhoneRevTree* pf, char const* num1, char const* num2, int depth){
    if(num2[depth] == '\0'){
        phnumAdd(pf->fwds, num1);
        return true;
    }
    if(pf->lower[getIndex(num2[depth])] == NULL){
        pf->lower[getIndex(num2[depth])] = phrevTreeNew();
        if(pf->lower[getIndex(num2[depth])] == NULL)
            return false;
    }
    return phfwdRecAddRev(pf->lower[getIndex(num2[depth])], num1, num2, depth + 1);
}

static bool phfwdRecAdd(PhoneFwdTree* pf, char const* num1, char const* num2, int depth){
    if(num1[depth] == '\0'){
        free(pf->fwd);
        pf->fwd = malloc((strlen(num2) + 1) * sizeof(char));
        if(pf->fwd == NULL)
            return false;
        strcpy(pf->fwd, num2);
        return true;
    }
    if(pf->lower[getIndex(num1[depth])] == NULL){
        pf->lower[getIndex(num1[depth])] = phfwdTreeNew();
        if(pf->lower[getIndex(num1[depth])] == NULL)
            return false;
    }
    return phfwdRecAdd(pf->lower[getIndex(num1[depth])], num1, num2, depth + 1);
}

bool phfwdAdd(PhoneForward *pf, char const *num1, char const *num2){
    if(pf == NULL || num1 == NULL || num2 == NULL)
        return false;
    if(strlen(num2) == 0 || strlen(num1) == 0)
        return false;
    if(strcmp(num1, num2) == 0)
        return false;
    int i = 0;
    while(num1[i] != '\0'){
        if(!isPhDigit(num1[i]))
            return false;
        i++;
    }
    i = 0;
    while(num2[i] != '\0'){
        if(!isPhDigit(num2[i]))
            return false;
        i++;
    }
    if(phfwdRecAdd(pf->treeFwd, num1, num2, 0) == false){
        return false;
    }
    return phfwdRecAddRev(pf->treeRev, num1, num2, 0);
}

static bool phrevDeletable(PhoneRevTree* t){
    if(t == NULL)
        return false;
    bool ans = true;
    if(phnumSize(t->fwds) > 0)
        ans = false;
    for(int i = 0; i < DIGITS; i++){
        if(t->lower[i] != NULL)
            ans = false;
    }
    return ans;
}

static bool phfwdDeletable(PhoneFwdTree* t){
    if(t == NULL)
        return false;
    bool ans = true;
    if(t->fwd != NULL)
        ans = false;
    for(int i = 0; i < DIGITS; i++){
        if(t->lower[i] != NULL)
            ans = false;
    }
    return ans;
}

//  Usuwa z list zrodel num2 numer num1
static void phfwdRecRemoveFromFwds(PhoneRevTree* revTree, char const* num1, char const* num2, int depth){
    if(num2[depth] == '\0'){
        phnumRemove(revTree->fwds, num1);
        return;
    }
    if(revTree->lower[getIndex(num2[depth])] != NULL){
        phfwdRecRemoveFromFwds(revTree->lower[getIndex(num2[depth])], num1, num2, depth + 1);
        if(phrevDeletable(revTree->lower[getIndex(num2[depth])])){
            phrevTreeDelete(revTree->lower[getIndex(num2[depth])]);
            revTree->lower[getIndex(num2[depth])] = NULL;
        }
    }
}

static void phfwdRecRemoveFwd(PhoneForward* pf, PhoneFwdTree* treeFwd, char* num){
    if(treeFwd->fwd != NULL){
        phfwdRecRemoveFromFwds(pf->treeRev, num, treeFwd->fwd, 0);
        free(treeFwd->fwd);
        treeFwd->fwd = NULL;
    }
    for(int i = 0; i < DIGITS; i++){
        if(treeFwd->lower[i] != NULL){
            char* cpy = malloc(sizeof(char) * (strlen(num) + 2));
            strcpy(cpy, num);
            cpy[strlen(num)] = digits[i];
            cpy[strlen(num) + 1] = '\0';
            phfwdRecRemoveFwd(pf, treeFwd->lower[i], cpy);
            free(cpy);
        }
    }
}

static void phfwdRecRemove(PhoneForward* pf, PhoneFwdTree* treeFwd, char const* num, int depth){
    if(num[depth] == '\0'){
        if(treeFwd->fwd != NULL)
            phfwdRecRemoveFromFwds(pf->treeRev, num, treeFwd->fwd, 0);
        free(treeFwd->fwd);
        treeFwd->fwd = NULL;
        for(int i = 0; i < DIGITS; i++){
            if(treeFwd->lower[i] != NULL){
                char* cpy = malloc(sizeof(char) * (strlen(num) + 2));
                strcpy(cpy, num);
                cpy[strlen(num)] = digits[i];
                cpy[strlen(num) + 1] = '\0';
                phfwdRecRemoveFwd(pf, treeFwd->lower[i], cpy);
                free(cpy);
                phfwdTreeDelete(treeFwd->lower[i]);
                treeFwd->lower[i] = NULL;
            }
        }
        return;
    }
    if(treeFwd->lower[getIndex(num[depth])] != NULL){
        phfwdRecRemove(pf, treeFwd->lower[getIndex(num[depth])], num, depth + 1);
        if(phfwdDeletable(treeFwd->lower[getIndex(num[depth])])){
            phfwdTreeDelete(treeFwd->lower[getIndex(num[depth])]);
            treeFwd->lower[getIndex(num[depth])] = NULL;
        }
    }
}

void phfwdRemove(PhoneForward *pf, char const *num){
    if(pf == NULL || num == NULL)
        return;
    for(size_t i = 0; i < strlen(num); i++){
        if(!isPhDigit(num[i]))
            return;
    }
    phfwdRecRemove(pf, pf->treeFwd, num, 0);
}

static char* phfwdRecGet(PhoneFwdTree const* pf, char const* num, int depth,
                            char* fwd_found, int fwd_found_depth){
    if(num[depth] == '\0'){
        /* Koniec numeru. */
        /* Jest przekierowanie. */
        char* ans;
        if(pf->fwd != NULL){
            ans = malloc((strlen(pf->fwd) + 1) * sizeof(char));
            if(ans == NULL)
                return NULL;
            strcpy(ans, pf->fwd);
        /* Brak przekierowania, ale znaleziono jakieś wcześniej. */
        } else if(fwd_found != NULL){
            ans = malloc((strlen(fwd_found) + strlen(num) - fwd_found_depth + 1) * sizeof(char));
            size_t i = 0;
            for(; i < strlen(fwd_found); i++)
                ans[i] = fwd_found[i];
            size_t j = 0;
            for(; j + fwd_found_depth < strlen(num); j++)
                ans[i + j] = num[fwd_found_depth + j];
            ans[i + j] = '\0';
        /* Nie znaleziono żadnego przekierowania. */
        } else{
            ans = malloc((strlen(num) + 1) * sizeof(char));
            if(ans == NULL)
                return NULL;
            strcpy(ans, num);
        }
        return ans;
    }
    /* Nie ma dalszej ścieżki. */
    if(pf->lower[getIndex(num[depth])] == NULL){
        char* ans;
        /* Jest przekierowanie. */
        if(pf->fwd != NULL){
            ans = malloc((strlen(pf->fwd) + 1 + strlen(num) - depth) * sizeof(char));
            size_t i = 0;
            for(; i < strlen(pf->fwd); i++)
                ans[i] = pf->fwd[i];
            size_t j = 0;
            for(; j + depth < strlen(num); j++)
                ans[i + j] = num[depth + j];
            ans[i + j] = '\0';
        /* Brak przekierowania, ale znaleziono jakieś wcześniej. */
        } else if(fwd_found != NULL){
            ans = malloc((strlen(fwd_found) + strlen(num) - fwd_found_depth + 1) * sizeof(char));
            size_t i = 0;
            for(; i < strlen(fwd_found); i++)
                ans[i] = fwd_found[i];
            size_t j = 0;
            for(; j + fwd_found_depth < strlen(num); j++)
                ans[i + j] = num[fwd_found_depth + j];
            ans[i + j] = '\0';
        /* Brak przekierowania. */
        } else{
            ans = malloc((strlen(num) + 1) * sizeof(char));
            if(ans == NULL)
                return NULL;
            strcpy(ans, num);
        }
        return ans;
    }
    if(pf->fwd != NULL)
        return phfwdRecGet(pf->lower[getIndex(num[depth])], num, depth + 1, pf->fwd, depth);
    return phfwdRecGet(pf->lower[getIndex(num[depth])], num, depth + 1, fwd_found, fwd_found_depth);
}

PhoneNumbers * phfwdGet(PhoneForward const *pf, char const *num){
    if(pf == NULL)
        return NULL;
    if(num == NULL || strlen(num) == 0){
        PhoneNumbers* ans = phnumNew();
        phnumAdd(ans, NULL);
        return ans;
    }
    bool is_num = true;
    for(size_t i = 0; i < strlen(num); i++)
        if(!isPhDigit(num[i]))
            is_num = false;
    if(!is_num){
        PhoneNumbers* ans = phnumNew();
        phnumAdd(ans, NULL);
        return ans;
    }
    char* fwd = phfwdRecGet(pf->treeFwd, num, 0, NULL, 0);
    PhoneNumbers* ans = phnumNew();
    phnumAdd(ans, fwd);
    free(fwd);
    return ans;
}

static PhoneNumbers* phfwdRecReverse(PhoneRevTree const *pf, char const * num, int depth, PhoneNumbers* pnum){
    if(pf->fwds != NULL){
        for(int i = 0; i < pf->fwds->cnt; i++){
            char* cpy = malloc(sizeof(char) * (strlen(pf->fwds->nums[i]) + 1 + strlen(num) - depth));
            int j = 0;
            for(; j < (int)strlen(pf->fwds->nums[i]); j++){
                cpy[j] = pf->fwds->nums[i][j];
            }
            for(int k = depth; k < (int)strlen(num); k++){
                cpy[j] = num[k];
                j++;
            }
            cpy[j] = '\0';
            phnumAdd(pnum, cpy);
            free(cpy);
        }
    }
    if(num[depth] == '\0')
        return pnum;
    if(pf->lower[getIndex(num[depth])] == NULL)
        return pnum;
    return phfwdRecReverse(pf->lower[getIndex(num[depth])], num, depth + 1, pnum);
}

PhoneNumbers * phfwdReverse(PhoneForward const *pf, char const *num){
    if(pf == NULL)
        return NULL;
    if(num == NULL || num[0] == '\0'){
        PhoneNumbers* pnum = phnumNew();
        phnumAdd(pnum, NULL);
        return pnum;
    }
    for(int i = 0; i < (int)strlen(num); i++){
        if(!isPhDigit(num[i])){
            PhoneNumbers* pnum = phnumNew();
            phnumAdd(pnum, NULL);
            return pnum;
        }
    }
    PhoneNumbers* pnum = phnumNew();
    phnumAdd(pnum, num);
    pnum = phfwdRecReverse(pf->treeRev, num, 0, pnum);
    phnumSort(pnum);
    return pnum;
}

PhoneNumbers* phfwdGetReverse(PhoneForward const *pf, char const *num){
    if(pf == NULL)
        return NULL;
    if(num == NULL || strlen(num) == 0){
        PhoneNumbers* pnum = phnumNew();
        phnumAdd(pnum, NULL);
        return pnum;
    }
    int is_num = 1;
    for(int i = 0; i < (int)strlen(num); i++){
        if(!isPhDigit(num[i]))
            is_num = 0;
    }
    if(is_num == 0){
        PhoneNumbers* p = phnumNew();
        return p;
    }
    PhoneNumbers* pnum = phfwdReverse(pf, num);
    PhoneNumbers* ans = phnumNew();
    for(int i = 0; i < phnumSize(pnum); i++){
        const char* candidate = phnumGet(pnum, i);
        PhoneNumbers* temp = phfwdGet(pf,candidate);
        const char* fwd = phnumGet(temp, 0);
        if(strcmp(fwd, num) == 0)
            phnumAdd(ans, candidate);
        phnumDelete(temp);
    }
    phnumDelete(pnum);
    return ans;
}
