#include <fstream>
#include <vector>

int main() {
    /*
    1 yes
    2 yes
    3 no
    */

    std::ifstream in("input.txt");
    std::ofstream out("output.txt");

    int n; in >> n; // считывание данных
    std::vector<int> min_binary_heap(n);
    for (int i = 0; i < n; ++i)
        in >> min_binary_heap[i];

    bool answer = true; // сама проверка на бинарно-кучность
    for (int i = 0; i < (n / 2) and answer == true; ++i) {
        if (((i << 1) + 1) < n && min_binary_heap[i] > min_binary_heap[((i << 1) + 1)]) 
            answer = false;
        if (((i << 1) + 2) < n && min_binary_heap[i] > min_binary_heap[((i << 1) + 2)])
            answer = false;
    }
    // они важны, когда -- ну если 2 * i + 2 элемента нет, но есть 2*i +1, то -- Мои проверки его не учтут
    // а добавив всё же Эти проверки, учтут
    out << (answer ? "Yes" : "No"); // вывод данных
    
    in.close(); out.close();
    return 0;
}