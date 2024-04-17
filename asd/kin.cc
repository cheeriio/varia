#include <iostream>

using namespace std;

const int MOD = 1000000000;

struct IntervalTree{
    int size; // more like max val but whatever
    int* tab;
};

IntervalTree newTree(int s){
    IntervalTree t;
    t.size = s;
    int* tt = (int*)malloc((2 * s - 1) * sizeof(int));
    for(int i = 0; i < 2 * s - 1; i++)
        tt[i] = 0;
    t.tab = tt;
    return t;
}

int parent(int n){
    return n / 2;
}

int left(int n){
    return 2*n;
}

int right(int n){
    return 2*n + 1;
}

void insertTree(int n, int val, IntervalTree & t){
    int s = 1, p = 1, k = t.size;
    while(s < 2 * t.size - 1){
        t.tab[s - 1] += val;
        t.tab[s - 1] %= MOD;
        if(n <=  p + (k - p) / 2){
            k = p + (k - p) / 2;
            s = left(s);
        } else{
            p = p + (k - p) / 2 + 1;
            s = right(s);
        }
    }
}

long long helper(int* tab, int p, int k, int ip, int ik, int par){
    if(p == ip && k == ik)
        return tab[par - 1];
    if(k <= ip + (ik - ip) / 2){ // po lewej
        return helper(tab, p, k, ip, ip + (ik - ip)/2, left(par));
    } else if(p <= ip + (ik - ip) / 2 && k > ip + (ik - ip) / 2){ // mieszane
        long long sum = 0;
        sum += helper(tab, p, ip + (ik - ip)/2, ip, ip + (ik - ip)/2, left(par));
        sum %= MOD;
        sum += helper(tab, ip + (ik - ip) / 2 + 1, k, ip + (ik - ip)/2 + 1, ik, right(par));
        sum %= MOD;
        return sum;
    } else{ // po prawej
        return helper(tab, p, k, ip + (ik - ip)/2 + 1, ik, right(par));
    }
}

int sumTree(int p, int k, IntervalTree & t){
    return helper(t.tab, p, k, 1, t.size, 1) % MOD;
}

int main(){
        int n, k;
        cin >> n >> k;
        int size = 1;
        while(size < n)
            size *= 2;

        long perm[n];
        long long *a, *b; // a na poczatku - 1 - inwersje
        a = (long long*)malloc(n * sizeof(long long));
        b = (long long*)malloc(n * sizeof(long long));

        for(int i = 0; i < n; i++){
            cin >> perm[i];
            a[i] = 1;
        }

        for(int i = 2; i <= k; i++){
            IntervalTree t = newTree(size); 
            for(int j = 0; j < n; j++){
                b[j] = sumTree(perm[j], size, t) % MOD;
                insertTree(perm[j], a[j], t);
            }
            free(a);
            a = b;
            b = (long long*)malloc(n * sizeof(long long));
        }
        long long ans = 0;
        for(int i = 0; i < n; i++)
            ans = (ans + a[i]) % MOD;
        
        cout << ans;

        return 0;
}
