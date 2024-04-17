#include <iostream>
#include <set>
#include <unordered_set>

using namespace std;
using pii = pair<int, int>;

struct cmp {
    bool operator() (pii a, pii b) const {
        if(a.second < b.second)
            return true;
        else if(a.second > b.second)
            return false;
        else
            return a.first < b.first;
    }
};

int main(){
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);

    int n, m, k;
    cin >> n >> m >> k;
    int p[n];
    bool used[n];

    set<pii, cmp> available;

    for(int i = 0; i < n; i++){
        cin >> p[i];
        available.insert({i, p[i]});
        used[i] = false;
    }

    set<int> needed[n];
    set<int> precedes[n];

    int a,b;
    for(int i = 0; i < m; i++){
        cin >> a >> b;
        needed[a - 1].insert(b - 1);
        precedes[b - 1].insert(a - 1);
        available.erase({a-1, p[a-1]});
    }

    int ans = 0;

    for(int i = 0; i < k; i++){
        pii opt = *(available.begin());
        used[opt.first] = true;
        available.erase(opt);
        ans = max(ans, opt.second);

        for(int following : precedes[opt.first]){
            needed[following].erase(opt.first);
            if(needed[following].empty() && !used[following])
                available.insert({following, p[following]});
        }
    }

    cout << ans;

    return 0;
}
