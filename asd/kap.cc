#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>

using namespace std;
using ll = long long;
using pll = pair<ll,ll>;

ll d(const pll &a, const pll &b){
	return min(abs(a.first - b.first), abs(a.second - b.second));
}

int main(){
	ios_base::sync_with_stdio(false);
	cin.tie(NULL);
	cout.tie(NULL);

	int n;
	cin >> n;
	vector<pll> nodes(n), xs(n), ys(n);

	for(int i = 0; i < n; i++){
		cin >> nodes[i].first >> nodes[i].second;
		xs[i] = make_pair(nodes[i].first, i);
		ys[i] = make_pair(nodes[i].second, i);
	}

	sort(xs.begin(), xs.end());
	sort(ys.begin(), ys.end());

	vector<pair<int, ll>> g[n];
	vector<ll> dist(n);
	for(int i = 1; i < n; i++){
		ll distance = d(nodes[xs[i-1].second], nodes[xs[i].second]);
		g[xs[i-1].second].emplace_back(xs[i].second, distance);
		g[xs[i].second].emplace_back(xs[i-1].second, distance);

		distance = d(nodes[ys[i-1].second], nodes[ys[i].second]);
		g[ys[i-1].second].emplace_back(ys[i].second, distance);
		g[ys[i].second].emplace_back(ys[i-1].second, distance);

		dist[i] = __INT32_MAX__;
	}
	dist[0] = 0;

	priority_queue<pair<ll, int>> q;
	q.emplace(0,0);

	while(q.size() > 0){
		pll v = q.top();
		q.pop();
		/*if(v.first != dist[v.second])
			continue;*/
		for(pll w : g[v.second]){
			if(w.second - v.first < dist[w.first]){
				dist[w.first] = w.second - v.first;
				q.emplace(v.first - w.second, w.first);
				//cout << "rlx" << " " << v.second << " " << w.first << endl;
			}
		}
	}

	cout << dist[n-1];
}
