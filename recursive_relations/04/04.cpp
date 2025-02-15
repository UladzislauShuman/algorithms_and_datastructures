#include <fstream>
#include <vector>
#include <climits>

void setDiamancionSequence(std::vector<int>& dimancion, const int& s, std::ifstream& in);
void findMin(const std::vector<int>& dimancion, std::vector<std::vector<int>>& dp, const int& i, const int& j);
int getMinAtomicMulOperations(const std::vector<int>& dimancion,const int& s);

int main() {
    std::ifstream in("input.txt");
    std::ofstream out("output.txt");

    int s; in >> s;
    std::vector<int> dimancion(s+1); setDiamancionSequence(dimancion, s,in);
    out << getMinAtomicMulOperations(dimancion,s);

    in.close(); out.close();
    return 0;
}

void setDiamancionSequence(std::vector<int>& dimancion, const int& s, std::ifstream& in)
{
    int last_dimancion;
    for (int i = 0; i < s;++i)
    {
        in >> dimancion[i];
        in >> last_dimancion;
        /*
        2 x
        3 x
        5 10*/
    }
    dimancion[s] = last_dimancion;
}

int getMinAtomicMulOperations(const std::vector<int>& dimancion, const int& s)
{
    std::vector<int> zero_vector(s, 0);
    std::vector<std::vector<int>> dp (s, zero_vector); // s*s из нулей
    int temp;

    //от 0 до s-1
    //j идёт по Строке, а i по СтроКам
    // обход снизу вверх
    for (int j = 1; j < s; ++j)// в j=0 у нас 0
    {
        dp[j-1][j] = dimancion[j-1] * dimancion[j] * dimancion[j+1];
        for (int i = j-1; i >=0; --i)//в j=i у нас dp[i][i] = 0
            findMin(dimancion,dp,i,j);
    }
    return dp[0][s-1]; 
}

void findMin(const std::vector<int>& dimancion, std::vector<std::vector<int>>& dp, const int& i, const int& j)
{
    dp[i][j] = INT_MAX;
    int temp;
    for (int k = i; k < j;++k)
    {
        temp = dp[i][k] + dp[k+1][j] + dimancion[i] * dimancion[k+1] * dimancion[j+1];
        if (temp < dp[i][j]) dp[i][j] = temp;
    }
}
