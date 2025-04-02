#include <fstream>
#include <vector>
#include <iostream>
#include <stack>

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
1 2 6 5 3 4 7
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
    int v; bool v_is_top;
    std::vector<int> last_visited_on_v(n,0);

    // пропишу одну из итераций
    for (int i = 0; i < n; ++i ) { // надо с целью чтобы пройти Все вершины
        if (answer[i] == 0) { // если не посещали, то посетим
            std::stack<int> stack; 
            stack.push(i); // 1 / 
            answer[i] = index++; // 1 
            while (!stack.empty()) {
                v = stack.top(); // top = 1 // вот теперь тут 2 И last_[] будет теперь для двойки
                v_is_top = true; 
                while (last_visited_on_v[v] < graph[v].size()) { // l[0]=0 < 2? 
                    int u = graph[v][last_visited_on_v[v]]; // g[1][l[0]] = 2;
                    last_visited_on_v[v]++; //l[0] = 1
                    if (answer[u] == 0) { // 2 не посещали
                        stack.push(u); // в стек
                        answer[u] = index++; // присваиваем индекс
                        v_is_top = false; // это не конец был, может есть куда углубиться
                        break;
                    }
                }
                if (v_is_top) { // в нашей итерации это не глубина, ничего не удаляем
                    stack.pop();
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

