#include <fstream>
#include <vector>
#include <algorithm>
#include <iostream>

int fun(int a, int b, std::vector<std::vector<int>>& hospital_ward, int index);

int main() {
    std::ifstream in("input1.txt");
    std::ofstream out("output.txt");

    int a; in >> a;
    int b; in >> b;
    int p; in >> p;

    std::vector<std::vector<int>> hospital_ward(p, std::vector<int>(3,0));
    for (int i = 0; i < p; ++i) 
    {
        in >> hospital_ward[i][0];
        in >> hospital_ward[i][1];
        in >> hospital_ward[i][2];
    }

    std::cout << a + b - fun(a,b, hospital_ward, 0);

    in.close(); out.close();
    return 0;
}

int fun(int a, int b, std::vector<std::vector<int>>& hospital_ward, int index)
{
    a = a < 0 ? 0 : a;
    b = b < 0 ? 0 : b;
    if (index >= hospital_ward.size())
        return a+b;
    if (hospital_ward[index][1] != 0) 
    {
        a -= (hospital_ward[index][0] - hospital_ward[index][1]);
        return fun(a, b, hospital_ward, index + 1);
    }
    else 
    {
        if (hospital_ward[index][2] != 0) // p_a == 0 and p_b != 0
        {
            b -= (hospital_ward[index][0] - hospital_ward[index][2]);
            return fun(a, b, hospital_ward, index + 1);
        }
        else { // p_a == 0 and p_b == 0
            // не продолжать разветвлять, когда a или b уже равны нулю
            //...
            int L = fun(a - hospital_ward[index][0], b, hospital_ward, index + 1);
            int R = fun(a, b - hospital_ward[index][0], hospital_ward, index + 1);
            return std::min(L, R);
        }
    }
}