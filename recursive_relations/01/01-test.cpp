#include <fstream>
#include <vector>
#include <iostream>
int main()
{

    std::ifstream inStream("input.txt");
    std::ofstream outStream("output.txt");

    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    const short MIN_MOSQUITO = -1002;

    int n; inStream >> n;
    
    if (n == 2){
        outStream << "-1";
        return 0;
    }
    
    std::vector<int> mosquitos(n);
    for (int i = 0; i < n; ++i)
        inStream >> mosquitos[i];

    
    if (n == 1){
        outStream << mosquitos[0] << "\n1";
        return 0;
    }
    // n >=1 , а в результате проверок ранее можно сказать, что n > 2
    std::vector<int> dynamic_programming_table(n,MIN_MOSQUITO);
    // далее в for, если i = 1, то будет dynamic_programming_table[i] = -1001 И складываем с mosquitos[i + 2]. сумма <= -1 < 0 
    std::vector<int> rout(n); rout[0] = -1; //наш маркер
    
    //заполняем dynamic_programming_table
    //ДП вперёд


    dynamic_programming_table[0] = mosquitos[0];
    for (int i = 0, temp; i < n; ++i)
    {
        if (i + 3 < n)
        {
            temp = dynamic_programming_table[i] + mosquitos[i + 3];
            if (dynamic_programming_table[i + 3] < temp) {
                rout[i + 3] = i;
                dynamic_programming_table[i + 3] = temp;
            }
        }
        if (i + 2 < n)
        {
            temp  = dynamic_programming_table[i] + mosquitos[i + 2];
            if (dynamic_programming_table[i + 2] < temp) {
                rout[i + 2] = i;
                dynamic_programming_table[i + 2] = temp;
            }
        } 
    }

    //если не допрыгал
    if (dynamic_programming_table[n - 1] == MIN_MOSQUITO)
    {
        outStream << "-1";
        return 0;
    }

    
    // ищем путь
    std::vector<int> result_route; 
    result_route.reserve((n + 1)>>1);// в худшем случае, она будет прыгать через 1 (1 3 5 ...)
    int i =  n - 1;
    while (i != -1) { //на маркер
        result_route.push_back(i + 1);
        i = rout[i];
    }

     //если не допрыгал
    if (result_route[result_route.size() - 1] != 1)
    {
        outStream << "-1";
        return 0;
    }

    //вывод результата
    //result_route хранит в себе путь в reverse
    outStream << dynamic_programming_table[n-1] << "\n"; // сколько съел лягушек
    for (auto it = result_route.rbegin(); it != result_route.rend(); ++it) {
        outStream << *it << " ";
    }
    inStream.close(); outStream.close();
    return 0;
}