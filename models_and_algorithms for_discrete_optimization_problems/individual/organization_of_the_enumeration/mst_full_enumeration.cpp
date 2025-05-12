#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>

using namespace std;

struct Edge {
    int u, v, weight, id;
};

int n, m;
vector<Edge> edges;
vector<vector<int>> all_msts;

// Проверка, является ли подмножество рёбер остовным деревом
bool is_spanning_tree(const vector<int>& subset) {
    vector<int> parent(n+1);
    for (int i = 1; i <= n; i++) parent[i] = i;
    
    auto find = [&](int u) {
        while (parent[u] != u) u = parent[u];
        return u;
    };
    
    for (int idx : subset) {
        Edge e = edges[idx];
        int root_u = find(e.u);
        int root_v = find(e.v);
        if (root_u == root_v) return false; // Найден цикл
        parent[root_v] = root_u;
    }
    
    // Проверяем связность
    int root = find(1);
    for (int i = 2; i <= n; i++)
        if (find(i) != root) return false;
    
    return true;
}

// Рекурсивный перебор всех комбинаций рёбер
void generate_combinations(int start, int k, vector<int>& current) {
    if (current.size() == k) {
        if (is_spanning_tree(current)) {
            all_msts.push_back(current);
        }
        return;
    }
    
    for (int i = start; i < m; i++) {
        current.push_back(i);
        generate_combinations(i+1, k, current);
        current.pop_back();
    }
}

int main() {
    ifstream in("test1.txt");
    ofstream out("output.txt");

    in >> n >> m;
    edges.resize(m);
    
    for (int i = 0; i < m; i++) {
        in >> edges[i].u >> edges[i].v >> edges[i].weight;
        edges[i].id = i+1;
    }

    // Генерируем все остовные деревья
    vector<int> current;
    generate_combinations(0, n-1, current);

    // Находим минимальный вес
    int min_weight = 10000000000000000;// INT_MAX 
    for (const auto& mst : all_msts) {
        int weight = 0;
        for (int idx : mst) weight += edges[idx].weight;
        if (weight < min_weight) min_weight = weight;
    }

    // Собираем все MST с минимальным весом
    vector<vector<int>> result;
    for (const auto& mst : all_msts) {
        int weight = 0;
        for (int idx : mst) weight += edges[idx].weight;
        if (weight == min_weight) {
            vector<int> ids;
            for (int idx : mst) ids.push_back(edges[idx].id);
            result.push_back(ids);
        }
    }

    // Вывод результатов
    out << result.size() << endl;
    for (const auto& mst : result) {
        for (int id : mst) out << id << " ";
        out << endl;
    }

    return 0;
}