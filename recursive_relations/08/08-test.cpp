#include <fstream>
#include <vector>
#include <string>
#include <algorithm>

void readData(
    std::ifstream&in ,
    int& x, int& y, int& z,
    std::string& A, std::string& B
);

int getMinAtoB(
    const int& Delete,const int& Insert,const int& Replace,
    const std::string& A,const std::string& B
);
int main() {
    std::ifstream in("in.txt");
    std::ofstream out("out.txt");

    int Delete,Insert,Replace;
    std::string A, B;
    
    readData(in, Delete, Insert, Replace, A, B);
    Replace = Replace < Delete + Insert? Replace : Delete + Insert;
    out << getMinAtoB(Delete, Insert, Replace, A, B);
    
    in.close(); out.close();
    return 0;
}

void readData(
    std::ifstream& in ,
    int& x, int& y, int& z,
    std::string& A, std::string& B
)
{
    in >> x >> y >> z;
    in >> A >> B;
}

int getMinAtoB(
    const int& Delete,const int& Insert,const int& Replace,
    const std::string& A,const std::string& B
)
{
    std::vector<std::vector<int>> dp(A.size() + 1, std::vector<int>(B.size() + 1));
    dp[0][0] = 0; 

    // мы хотим A превратить в B  
    /*
              A = пустая строка
              Bj-1 = b1b2...bj-1
              bj был добавлен => Bj = Bj-1 + yj
              т.е. A нужно преобразовать в Bj
              т.е. пустую строку нужно заполнить элементами
              т.е. Insert в неё
              если до этого для Bj-1 нам надо было добавить 
              dp[0][j-1]
              то теперь нам добавить и заплатить Insert 
            */
    for (int i = 1; i <= B.size(); ++i) // вероятно предыдущий объеденённый цикл и стал причиной RTE
        dp[0][i] = dp[0][i - 1] + Insert;
        
    /*
        B = пустая
        A = a1...ai
        для того, чтобы A превратить в пустую строку, нужно
        удалять элементы из неё
        */
    for (int i = 1; i <= A.size(); ++i)
        dp[i][0] = dp[i - 1][0] + Delete;
    
    for (int i = 1; i <= A.size(); ++i)
    {
        for (int j = 1; j <= B.size(); ++j)
        {
            /*
                dp[i][j] -- это min количество штрафов, которые
                нужно заплатить чтобы из A = a1...ai сделать B = b1...bj
                получить можно различными путями
                1)Ai-1 --> Bj, то Ai --> Bj = (Ai-1 --> Bj) + DeleteFromA(ai)
                2)Ai --> Bj-1, т.е. Ai --> Bj = (Ai --> Bj-1) + InsertInA(bj)
                3)Ai-1 --> Bj-1 И, если ai == bj, то никаких штрафов платить не надо,
                либо если не равны, то можно сделать равными --  ReplaceInA(from: ai, to: bj)
                и тот, что стоит дешевле, тот нам и подходит
            */
            dp[i][j] = std::min({
                    dp[i-1][j] + Delete,//1
                    dp[i][j-1] + Insert,//2
                    dp[i-1][j-1] + (A[i-1] == B[j-1]? 0 : Replace)//3 
            });
        }
    }
    return dp[A.size()][B.size()];
}