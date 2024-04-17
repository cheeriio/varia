#include <iostream>
#include <algorithm>
#include <vector>

using namespace std;
using Pii = pair<int, int>; // (liczba członków komitetu, liczba takich komitetów)

const int MOD = 1000000007;
int n;
vector<Pii> tree;
int tsize = 1;

Pii sum(Pii a, Pii b)
{
  if (a.first == b.first)
    return {a.first, (a.second + b.second) % MOD};
  else if (a.first < b.first)
    return a;
  else
    return b;
}

void insert(int i, Pii v)
{
  i += tsize - 1;
  tree[i] = v;
  i /= 2;
  while (i > 0)
  {
    tree[i] = sum(tree[2 * i], tree[2 * i + 1]);
    i /= 2;
  }
}

Pii get(int l, int r)
{
  Pii def = {n + 1, 0};
  l += tsize - 1;
  r += tsize - 1;
  while (l <= r)
  {
    // Prawy syn
    if (l % 2 == 1)
    {
      def = sum(def, tree[l]);
      l++;
    }
    // Lewy syn
    if (r % 2 == 0)
    {
      def = sum(def, tree[r]);
      r--;
    }
    l /= 2;
    r /= 2;
  }
  return def;
}

int main()
{
  int k, l;
  cin >> n >> k >> l;

  if(n == 1){
    cout << "1 1";
    return 0;
  }

  vector<int> vals(n);
  
  for (int i = 0; i < n; i++)
    cin >> vals[i];

  vals.push_back(-1000000001);
  sort(vals.begin(), vals.end());
  
  vector<Pii> dp(n + 1);
  dp[0] = {0, 1};   

  while (tsize < n + 1)
    tsize *= 2;

  tree = vector<Pii>(2 * tsize, {n + 1, 0});

  insert(0, {0, 1}); // Odpowiedź dla pustego zbioru pracowników.
  Pii solution = {n + 1, 0}; // Domyślna odpowiedź, która zawsze "przegrywa" w funkcji sum.

  for (int i = 1; i <= n; i++)
  {
    // Uzupełniamy dp. Ile jest optymalnych komitetów reprezentujących pracowników
    // o randze <= vals[i], takich że pracownik i-ty należy do komitetu?

    // Pracownik o największej randze niereprezentowany przez pracownika i-tego.
    int j = lower_bound(vals.begin(), vals.end(), vals[i] - k) - vals.begin() - 1;

    // Pracownik o najmniejszej randze, który reprezentuje pracownika j-tego.
    int left = lower_bound(vals.begin(), vals.end(), vals[j] - k) - vals.begin();

    // Pracownik o największej randze mniejszej od rangi pracownika i-tego, który może być w komitecie razem z i-tym.
    int right = lower_bound(vals.begin(), vals.end(), vals[i] - l + 1) - vals.begin() - 1;

    // Liczba komitetów do których dodajemy i-tego pracownika.
    dp[i]= get(left, right);
    dp[i].first++;
    insert(i, dp[i]);

    // Jeżeli i-ty pracownik reprezentuje pracownika o największej randze to bierzemy właśnie policzone
    // komitety pod uwagę, budując wynik.
    if (vals[i] + k >= vals[n])
      solution = sum(solution, dp[i]);
  }

  cout << solution.first << " " << solution.second;
}
