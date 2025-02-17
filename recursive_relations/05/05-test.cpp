#include <fstream>
#include <vector>

void readAB(std::vector<int>& a, std::vector<int>& b, int& n, std::ifstream& in);
void setOut(std::vector<int>& lsc,  std::ofstream& out);
int main() {
    std::ifstream in("input.txt");
    std::ofstream out("output.txt");

    int n; in >> n;
    std::vector<int> a(n,0);
    std::vector<int> b(n,0);
    readAB(a, b, n, in);

    //получаем dp И длинну наибольшей подпоследовательности
    std::vector<std::vector<int>> dp(n+1, std::vector<int>(n+1, 0));
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
    out << dp[n][n] << "\n";
    
    //получаем индексы элементов в a и b, из которых и составлена наибольшая подпоследовательность
    std::vector<int> lsc_a,lsc_b;
    lsc_a.reserve(n); lsc_b.reserve(n);

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

    //выводим
    setOut(lsc_a, out); setOut(lsc_b, out);

    in.close();
    out.close();
    return 0;
}

void readAB(std::vector<int>& a, std::vector<int>& b, int& n, std::ifstream& in)
{
    for (int i = 0; i < n; ++i)
        in >> a[i];
    for (int i = 0; i < n; ++i) 
        in >> b[i];
}

void setOut(std::vector<int>& lsc, std::ofstream& out)
{
    for (auto iterator = lsc.rbegin(); iterator!= lsc.rend(); ++iterator)
        out << *iterator << " ";
    out << "\n";
}