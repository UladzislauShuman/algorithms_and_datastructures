#include <vector>
#include <iostream>

int main()
{
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n; std::cin >> n;
    if (n < 3){
        std::cout << "-1";
        return 0;
    }
    
    std::vector<int> mosquitos(n);
    for (int i = 0; i < n; ++i)
        std::cin >> mosquitos[i];

    std::vector<int> dynamic_programming_table(n,0); dynamic_programming_table[1] = -1001;
    // далее в for, если i = 1, то будет dynamic_programming_table[i] = -1001 И складываем с mosquitos[i + 2]. сумма <= -1 < 0 
    std::vector<int> rout(n); rout[0] = -1; //наш маркер
    
    //заполняем dynamic_programming_table
    //ДП вперёд


    dynamic_programming_table[0] = mosquitos[0];
    for (int i = 0, temp; i < n; ++i)
    {
        if (i + 3 < n)
        {
            temp  = dynamic_programming_table[i] + mosquitos[i + 2];
            if (dynamic_programming_table[i + 2] < temp) {
                rout[i + 2] = i;
                dynamic_programming_table[i + 2] = temp;
            }

            temp = dynamic_programming_table[i] + mosquitos[i + 3];
            if (dynamic_programming_table[i + 3] < temp) {
                rout[i + 3] = i;
                dynamic_programming_table[i + 3] = temp;
            }
        }
        else
            if (i + 2 < n){
                temp  = dynamic_programming_table[i] + mosquitos[i + 2];
                if (dynamic_programming_table[i + 2] < temp) {
                    rout[i + 2] = i;
                    dynamic_programming_table[i + 2] = temp;
                }
            } 
            else
                break;
    }

    std::cout << dynamic_programming_table[n-1] << "\n"; // сколько съел лягушек
    
    // ищем путь
    std::vector<int> result_route((n - 1)>>1 + 2); // в худшем случае, она будет прыгать через 1 (1 3 5 ...)
    int i =  n - 1;
    while (i != -1) { //на маркер
        result_route.push_back(i + 1);
        i = rout[i];
    }

    //result_route хранит в себе путь в reverse
    for (auto it = result_route.rbegin(); it != result_route.rend(); ++it) {
        std::cout << *it << " ";
    }

    return 0;
}