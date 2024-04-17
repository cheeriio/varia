#include <iostream>

using namespace std;

// adj - used when entering lower branches, affected by earlier inserts
int tree[2*1048576], adj[2*1048576];

int left(int v){
    return 2*v;
}

int right(int v){
    return 2*v + 1;
}

void adjust(int v) {
    if(adj[v] == 0)
        return;
    if(adj[v] == 1){
        adj[left(v)] = 1;
        adj[right(v)] = 1;
        tree[left(v)] = 0;
        tree[right(v)] = 0;
    } else{
        adj[left(v)] = 2;
        adj[right(v)] = 2;
        tree[left(v)] = tree[right(v)] = tree[v]/2;
    }
    adj[v] = 0;
}

void insert(int a, int b, int val, int v, int p, int k) {
    if(p > b || k < a) // nothing to do here
        return;
    if(a <= p && k <= b){ // same intervals
        tree[v] = (k - p + 1) * val;
        adj[v] = val + 1;
        return;
    }
    adjust(v);
    int m = (p + k)/2;
    insert(a, b, val, left(v), p, m);
    insert(a, b, val, right(v), m + 1,  k);
    tree[v] = tree[left(v)] + tree[right(v)];
    return;
}

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);
    int n, m;
    cin >> n >> m;
    int a, b, upper = 1;
    char c;
    while(upper < n)
        upper *= 2;
    for (int i = 0; i < m; i++){
        cin >> a >> b >> c;
        if(c == 'B') 
            insert(a, b, 1, 1, 1, upper);
        if(c == 'C')
            insert(a, b, 0, 1, 1, upper);
        cout << tree[1] << "\n";
    }
}
