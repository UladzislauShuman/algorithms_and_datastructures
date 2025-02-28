#include <fstream>
#include <vector>
#include <limits>

const long long MAX_Int  = std::numeric_limits<long long>::max();
const long long MIN_Int  = std::numeric_limits<long long>::min();

/*
забыл, что в C++ нуль-индексация

изменение 2

вот на этом тесте
2
-2147483648
-2147483648 1 L

когда было int --  то вывод yes
когда было long long -- то no
и ведь -- правильно no

(m просто ограничен в пределе int -- и нельзя на 1 дальше или меньше отойти)
*/

int main() {
    std::ifstream in("bst.in");
    std::ofstream out("bst.out");
    // std::ifstream in("input5.txt");
    // std::ofstream out("output.txt");

    long long n; in >> n;
    
    std::vector<long long> data(n);
    std::vector<char> LR(n);
    std::vector<long long> min(n);
    std::vector<long long> max(n); 
    in >> data[0]; LR[0] = 'N'; min[0] = MIN_Int; max[0] = MAX_Int;// первый элемент

    long long p;
    for (int i = 1; i < n; ++i) {
        in >> data[i] >> p >> LR[i];
        // всё, что слева -- меньше, а то что спрва -- больше или равно
        //проверять мы будем всё время
        if (LR[i] == 'L') { // всё что слева, то меньше
            //слева от data[p]
            min[i] = min[p-1];
            max[i] = data[p-1] - 1;//тк на вход у нас целые числа, то чтобы a < b достачтоно чтобы a <= b - 1 
        }
        else if (LR[i] == 'R') { // всё что справа, то больше или ра��но
            //справа от data[p]
            max[i] = max[p-1];
            min[i] = data[p-1];
        }
        if (data[i] > max[i] or data[i] < min[i]) {
            out << "NO";
            in.close(); out.close();
            return 0;
        }
    }
    out << "YES";
    in.close(); out.close();
    return 0;
}