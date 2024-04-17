#include <iostream>
#include <set>
#include <vector>
#include <unordered_map>
#include <climits>
using namespace std;
int n, q, k = 0, tsize = 1;

void gen_mapa(int m[], set<int> sons[], int v)
{
  m[k++] = v;
  for (int s : sons[v])
  {
    gen_mapa(m, sons, s);
  }
}

struct bombka
{
  bool exists;
  bool correct;
  int good;
  int bad;
  int color;
  int bad_color;
};

void gen_bounds(set<int> sons[], int m[], vector<pair<int, int>> &bounds, int source)
{
  if (sons[source].size() == 0)
  {
    bounds[source] = {m[source], m[source]};
    return;
  }
  int low = m[source], upp = m[source];
  for (int s : sons[source])
  {
    gen_bounds(sons, m, bounds, s);
    low = min(low, bounds[s].first);
    upp = max(upp, bounds[s].second);
  }
  bounds[source] = {low, upp};
}

bombka merge(bombka a, bombka b)
{
  bombka ret;
  if (b.exists)
  { 
    if (a.correct && b.correct && a.bad + b.bad < 2 && a.color == b.color)
      ret = {true, true, a.good + b.good, a.bad + b.bad, a.color, 0};
    else if(a.good == 1 && b.good == 1 && a.bad == 0 && b.bad == 0 && a.color == b.color)
      ret = {true, true, 2, 0, a.color, 0};
    else if(a.good == 1 && b.good == 1 && a.bad == 0 && b.bad == 0 && a.color != b.color)
      ret = {true, true, 1, 1, a.color, b.color};
    else if (a.good == 1 && a.bad == 1 && b.correct && b.color == a.bad_color && b.bad == 0)
      ret = {true, true, a.bad + b.good, a.good, b.color, 0};
    else if (b.good == 1 && b.bad == 1 && a.correct && a.color == b.bad_color && a.bad == 0)
      ret = {true, true, b.bad + a.good, b.good, a.color, 0};
    else if (a.good == 1 && a.bad == 0 && b.bad == 0 && b.correct && a.color == b.color)
      ret = {true, true, b.good + 1, 0, b.color, a.color};
    else if (b.good == 1 && b.bad == 0 && a.bad == 0 && a.correct && a.color == b.color)
      ret = {true, true, a.good + 1, 0, a.color, b.color};
    else if (a.good == 1 && a.bad == 0 && b.bad == 0 && b.correct && a.color != b.color)
      ret = {true, true, b.good, a.good, b.color, a.color};
    else if (b.good == 1 && b.bad == 0 && a.bad == 0 && a.correct && a.color != b.color)
      ret = {true, true, a.good, b.good, a.color, b.color};
    else
      ret = {true, false, 0, 0, 0, 0};
  }
  else
  {
    ret = a;
  }
  return ret;
}

int main()
{
  cin >> n >> q;
  set<int> sons[n];
  int b;
  for (int i = 1; i < n; i++)
  {
    cin >> b;
    sons[b - 1].insert(i);
  }
  int mapa[n]; // która bombka na danej pozycji
  gen_mapa(mapa, sons, 0);
  int mapa2[n]; // która pozycja danej bombki

  for (int i = 0; i < n; i++)
    mapa2[mapa[i]] = i;

  vector<pair<int, int>> bounds(n, {0, 0});
  gen_bounds(sons, mapa2, bounds, 0);

  while (tsize < n)
    tsize *= 2;
  bombka def = {false, false, 0, 0, 0, 0};
  // INDEKS korzenia drzewa to 1 ! ! !
  vector<bombka> choinka(2 * tsize, def);
  for (int i = 0; i < n; i++)
  {
    choinka[tsize + mapa2[i]] = {true, true, 1, 0, 0, 0};
    cin >> choinka[tsize + mapa2[i]].color;
  }

  for (int i = tsize - 1; i > 0; i--)
  {
    choinka[i] = merge(choinka[2 * i], choinka[2 * i + 1]);
  }

  char c;
  int v, x;
  for (int j = 0; j < q; j++)
  {
    cin >> c >> v;
    if (c == 'z')
    {
      cin >> x;
      int i = tsize + mapa2[v - 1];
      choinka[i].color = x;
      while ((i /= 2) > 0)
      {
        choinka[i] = merge(choinka[2 * i], choinka[2 * i + 1]);
      }
    }
    else
    {
      int l = tsize + bounds[v - 1].first, r = tsize + bounds[v - 1].second;
      bombka ans;
      bool set = false;
      while (l <= r)
      {
        if (l % 2 == 1)
        {
          if (set)
            ans = merge(ans, choinka[l]);
          else
          {
            ans = choinka[l];
            set = true;
          }
          /*cout << l << " ";
          cout << "(" << choinka[l].correct << " " << choinka[l].color << " " << choinka[l].good << " " << choinka[l].bad << ") ";*/
          l++;
        }
        if (r % 2 == 0)
        {
          if (set)
            ans = merge(ans, choinka[r]);
          else
          {
            ans = choinka[r];
            set = true;
          }
          /*cout << r << " ";
          cout << "(" << choinka[r].correct << " " << choinka[r].color << " " << choinka[r].good << " " << choinka[r].bad << ") ";*/
          r--;
        }
        r /= 2;
        l /= 2;
      }
      if(ans.correct)
        cout << "TAK\n";
      else
        cout << "NIE\n";
    }

    /*int div = 1, cnt = 1;
    for(int k = 1; k < 2 * tsize; k++){
      cout << "(" << choinka[k].correct << " " << choinka[k].color << " " << choinka[k].bad << ") ";
      if(cnt % div == 0){
        cout << endl;
        div *= 2;
        cnt = 1;
      } else
        cnt++;
    }*/

  }
}
