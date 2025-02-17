#include <iostream>
#include <vector>

using namespace std;
void readAB(std::vector<int>& a, std::vector<int>& b, int& n, std::istream& in);
void makeDP(std::vector<std::vector<int>>& dp,std::vector<int>& a, std::vector<int>& b, int n);
void findRoute(std::vector<std::vector<int>>& dp, std::vector<int>& lsc_a, std::vector<int>& lsc_b, std::vector<int>& a, std::vector<int>& b, int n);
void setOut(std::vector<int>& lsc,  std::ostream& out);
int main() {

    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n; cin >> n;
    std::vector<int> a(n,0);
    std::vector<int> b(n,0);
    readAB(a, b, n, cin);

    //получаем dp И длинну наибольшей подпоследовательности
    std::vector<std::vector<int>> dp(n+1, std::vector<int>(n+1, 0));
    makeDP(dp, a, b, n);
    cout << dp[n][n] << "\n";
    
    //получаем индексы элементов в a и b, из которых и составлена наибольшая подпоследовательность
    std::vector<int> lsc_a,lsc_b;
    lsc_a.reserve(n); lsc_b.reserve(n);
    findRoute(dp, lsc_a, lsc_b, a, b, n);

    //выводим
    setOut(lsc_a, cout); setOut(lsc_b, cout);
    return 0;
}

void readAB(std::vector<int>& a, std::vector<int>& b, int& n, std::istream& in)
{
    for (int i = 0; i < n; ++i)
        in >> a[i];
    for (int i = 0; i < n; ++i) 
        in >> b[i];
}

void makeDP(std::vector<std::vector<int>>& dp,std::vector<int>& a, std::vector<int>& b, int n)
{
    for (int i = 1; i < n+1; ++i)
    {
        for (int j = 1; j < n+1; ++j)
        {
            if (a[i-1] == b[j-1]) //i-1 -- тк пробигаем мы по dp, а у него с a и b разница в единицу
                dp[i][j] = dp[i-1][j-1] + 1;
            else// a[i] != b[j]
                dp[i][j] = dp[i-1][j] > dp[i][j-1]? dp[i-1][j] : dp[i][j-1];
        }
    }
}

void findRoute(std::vector<std::vector<int>>& dp, std::vector<int>& lsc_a, std::vector<int>& lsc_b, std::vector<int>& a, std::vector<int>& b, int n)
{
    for (int i = n, j = n, len = dp[n][n]; len > 0;)
    {
        if (a[i-1] == b[j-1])
        {
            lsc_a.push_back(i-1);
            lsc_b.push_back(j-1);
            --i; --j;
            --len;
        }
        else if (dp[i-1][j] > dp[i][j-1])
            --i;
        else
            --j;
    }
}
void setOut(std::vector<int>& lsc, std::ostream& out)
{
    for (auto iterator = lsc.rbegin(); iterator!= lsc.rend(); ++iterator)
        out << *iterator << " ";
    out << "\n";
}