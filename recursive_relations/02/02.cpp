#include <iostream>
#include <vector>

using namespace std;

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    const int MOD_NUM = 1000000007;
    int n,k;
    cin >> n >> k;

    if (k > ((n+1)>>1)) 
    {
        k = n - k;
    }
    // увидив результат, я пошёл проверять через WolframAlpha, и получил разные ответы для 1000 500
    // вероятно это всё же связанно с погрешностью деления
    // но есть  ещё треугольнк Паскаля

    std::vector<int> up(n+1,0), down(n+1,0);
    up[0] = 1;
    for (int i = 0; i < n; ++i)
    {
        for (int j = 0; j <= i; ++j)
        {
            down[j] += up[j] % MOD_NUM;
            down[j+1] += up[j] % MOD_NUM;
        }
        std::swap(up,down);
        std::fill(down.begin(), down.end(), 0); //O(n)
    }
    cout << up[k] % MOD_NUM;
    return 0;
}