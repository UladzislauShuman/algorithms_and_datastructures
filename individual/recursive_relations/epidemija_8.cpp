#include <fstream>
#include <vector>


int main() {
    std::ifstream in("input.txt");
    std::ofstream out("output.txt");

    int a; in >> a;
    int b; in >> b;
    int p; in >> p;
    int aPlusb = a + b;

    std::vector<int> three_zeros(3,0);
    std::vector<std::vector<int>> hospital_ward(p, three_zeros);
    // считаем данные И разместим уже заведомо в палаты согласно типу, если в них кто-то уже есть
    for (int i = 0 ; i < p ; ++i)
    {
        for (int j = 0 ; j < 3; ++j)
            in >> hospital_ward[i][j];
        if (hospital_ward[i][1] > 0 and a > 0)
            a = a - (hospital_ward[i][0] - hospital_ward[i][1]);
        if (hospital_ward[i][2] > 0 and b > 0)
            b = b - (hospital_ward[i][0] - hospital_ward[i][2]);
    }   
    a = a > 0? a : 0;
    b = b > 0? b : 0;


    in.close(); out.close();
    return 0;
}