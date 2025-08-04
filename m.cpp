#include <bits/stdc++.h>
#define MAXN 200010
using namespace std;
map<int, int> vet;
int main()
{
    int n,x,act=0, mx=0; cin >> n;
    int inicio=1;
    for(int i=1; i <= n;i++)
    {
        cin >> x;
        if (vet[x] >= inicio)
        {
            mx = max(mx, act);
            inicio = vet[x]+1;
            act = i-inicio;
        }
        vet[x] = i;
        act++;
    }
    cout << max(mx, act)<< endl;
   return 0;
}