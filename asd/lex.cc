#include <iostream>
#include <vector>
using namespace std;
constexpr int MAX_N = 300007, q = 1000000007;
int main()
{
  int n, m;
  string s;
  cin >> n >> m >> s;
  vector<long long> h(n + 1, 0), p(n + 1, 1);
  auto HASH = [&](long long a, long long b, long long c) -> long long
  {
    return (((h[b] - h[a - 1] + q) * p[c]) % q);
  };
  for (long long i = 0; i < n; i++)
  {
    h[i + 1] = (h[i] + (s[i] - 'a') * p[i]) % q;
    p[i + 1] = (p[i] * 997) % q;
  }

  for (int a, b, c, d; m > 0; m--)
  {
    cin >> a >> b >> c >> d;
    bool change = false;
    if(a > c){
      change = true;
      int temp1 = a, temp2 = b;
      a = c;
      b = d;
      c = temp1;
      d = temp2;
    }
    int low = 0, high = min(b - a, d - c), diff = c - a;
    if (b - a == d - c && HASH(a, b, diff) == HASH(c, d, 0))
      cout << "=\n";
    
    else
    {
      while (low < high)
      {
        int mid = (low + high) / 2;
        if (HASH(a, a + mid, diff) == HASH(c, c + mid, 0))
          low = mid + 1;
        else
          high = mid;
      }
      if (s[a - 1 + low] == s[c - 1 + low])
      {
        if (b - a < d - c)
        {
          if (change)
            cout << ">\n";
          else
            cout << "<\n";
        }
        else
        {
          if (change)
            cout << "<\n";
          else
            cout << ">\n";
        }
      }
      else
      {
        if (s[a - 1 + low] < s[c - 1 + low])
        {
          if (change)
            cout << ">\n";
          else
            cout << "<\n";
        }
        else
        {
          if (change)
            cout << "<\n";
          else
            cout << ">\n";
        }
      }
    }
  }
}
