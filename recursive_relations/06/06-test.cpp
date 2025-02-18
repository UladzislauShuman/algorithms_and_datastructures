#include <fstream>
#include <vector>
#include <string>
#include <algorithm>

void makeDP(std::vector<std::vector<int>>& dp,const std::string& x,const int& n);
void returnPalindrome(
    std::string& result_palindrome, 
    const std::vector<std::vector<int>>& dp, 
    const std::string& x,
    const int& n,
    std::ofstream& out
);
void show(std::string& result_palindrome, const int& DP0N_1, std::ofstream& out);
int main() {
    std::ifstream in("input.txt");
    std::ofstream out("output.txt");

    std::string x; in >> x;
    int n = x.size();

    std::vector<std::vector<int>> dp(n, std::vector<int>(n, 0));
    makeDP(dp, x, n);
    std::string result_palindrome; result_palindrome.reserve(dp[0][n-1]);
    returnPalindrome(result_palindrome, dp, x, n, out);
    show(result_palindrome, dp[0][n-1], out);

    in.close();
    out.close();
    return 0;
}

void makeDP(std::vector<std::vector<int>>& dp,const std::string& x,const int& n)
{
    //будем идти по j, и идти снизу вверх
    for (int j = 0; j < n; ++j)
    {
        dp[j][j] = 1;
        for (int i = j-1; i >= 0; --i)
        {
            if (x[i] == x[j])
                dp[i][j] = dp[i + 1][j - 1] + 2;
            else
                dp[i][j] = dp[i + 1][j] > dp[i][j - 1] ? dp[i + 1][j] : dp[i][j - 1]; 
        }
    }
}

void returnPalindrome(
    std::string& result_palindrome, 
    const std::vector<std::vector<int>>& dp, 
    const std::string& x,
    const int& n,
    std::ofstream& out
)
{
    int i = 0, j = n - 1;
    while (i <= j)
    {   
        if (x[i] == x[j])
        {
            result_palindrome.push_back(x[i]);
            ++i; --j;
        }
        else if (dp[i + 1][j] > dp[i][j - 1])
            ++i;
        else
            --j;
    }
}

void show(std::string& result_palindrome, const int& DP0N_1, std::ofstream& out)
{
    out << DP0N_1 << "\n";
    out << result_palindrome;
    std::reverse(result_palindrome.begin(), result_palindrome.end());
    if (result_palindrome.size() * 2 == DP0N_1)
        out << result_palindrome;
    else 
        out << result_palindrome.substr(1);
    out << "\n";
}