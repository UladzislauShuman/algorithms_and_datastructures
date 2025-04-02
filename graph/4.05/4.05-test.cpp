#include <fstream>
#include <vector>
#include <iostream>
#include <queue>

/*
рассмотрим первый тест
7
0 1 1 0 0 0 0
0 0 0 0 1 0 0
0 1 0 1 1 0 0
0 1 0 0 1 0 0
0 0 0 0 0 1 0
0 0 0 1 0 0 0
0 0 0 0 0 1 0

ответ
1 2 3 5 4 6 7
*/
int main() {
    std::ifstream in("input.txt");
    std::ofstream out("output.txt");

    int n;
    in >> n;
    std::vector<std::vector<int>> graph(n);

    int temp;
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            in >> temp;
            if (temp == 1) {
                graph[i].push_back(j);
            }
        }
    }

    int index = 1;
    std::vector<int> answer(n, 0);

    for (int i = 0; i < n; ++i) { // нужно, чтобы мы прошли все вершины/компоненты связанности
        if (answer[i] == 0) {
            std::queue<int> queue;
            queue.push(i);
            answer[i] = index++;

            while (!queue.empty()) {
                int v = queue.front();queue.pop(); // если выписать очередь с L на R, то 

                for (int j : graph[v]) { // обходим смежные к v
                    if (answer[j] == 0) { // если не была посещена до этого
                        answer[j] = index++; // присваиваем индекс
                        queue.push(j); // и закидываем в очерель
                    }
                }
            }
        }
    }

    for (int i = 0; i < n; ++i) {
        out << answer[i] << " ";
    }

    in.close();
    out.close();
    return 0;
}

