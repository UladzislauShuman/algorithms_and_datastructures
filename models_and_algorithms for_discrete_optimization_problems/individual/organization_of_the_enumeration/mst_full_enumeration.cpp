#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>

using namespace std;

struct Edge {
    int u;
    int v;
    int weight;
    int id;
};

struct DSU 
{
    vector<int> parents;
    vector<int> rank; 
    vector<pair<int, int>> history_parents; // Для отката родителей
    vector<pair<int, int>> history_rank;    // Для отката рангов

    DSU(int n) {
        parents.resize(n + 1);
        rank.resize(n + 1, 1);
        for (int i = 1; i <= n; ++i) {
            parents[i] = i;
        }
    }

    int getParent(int v) {
        int parent = v;
        while (parent != parents[parent]) {
            parent = parents[parent];
        }
        while (v != parent) {
            int next = parents[v];
            parents[v] = parent;
            v = next;
        }
        return parent;
    }

    bool unite(int a, int b) {
        int parent_a = getParent(a);
        int parent_b = getParent(b);
        if (parent_a == parent_b) {
            return false; 
        }
       
        if (rank[parent_a] < rank[parent_b]) {
            swap(parent_a, parent_b);
        }
        // Сохраняем состояние перед изменением
        history_parents.push_back({parent_b, parents[parent_b]});
        history_rank.push_back({parent_a, rank[parent_a]});
        parents[parent_b] = parent_a;
        if (rank[parent_a] == rank[parent_b]) {
            rank[parent_a]++;
        }
        return true;
    }

    void doCtrlS() {
        // Сохраняем текущее состояние DSU
        history_parents.emplace_back(-1, -1); // Маркер начала сохранения
        history_rank.emplace_back(-1, -1);
    }

    void doCtrlZ() {
        // Восстанавливаем состояние до последнего сохранения
        while (!history_parents.empty()) {
            auto [v, prev_parent] = history_parents.back();
            history_parents.pop_back();
            if (v == -1) break; // Дошли до маркера
            parents[v] = prev_parent;
        }
        while (!history_rank.empty()) {
            auto [v, prev_rank] = history_rank.back();
            history_rank.pop_back();
            if (v == -1) break; // Дошли до маркера
            rank[v] = prev_rank;
        }
    }
};

// Объявления функций
bool isSpanningTree(const vector<Edge>& edges, int n, const vector<int>& subset);
void generateCombinations(const vector<Edge>& edges, int n, int m, int start,
                         vector<int>& current, vector<vector<int>>& allMsts);
int findMinWeight(const vector<Edge>& edges, const vector<vector<int>>& allMsts);
vector<vector<int>> filterMsts(const vector<Edge>& edges, 
                             const vector<vector<int>>& allMsts, int minWeight);
void printResults(const vector<vector<int>>& result, ostream& out);

int main() {
    ifstream in("test1.txt");
    ofstream out("output.txt");

    int n, m;
    in >> n >> m;
    
    vector<Edge> edges(m);
    for (int i = 0; i < m; i++) {
        in >> edges[i].u >> edges[i].v >> edges[i].weight;
        edges[i].id = i + 1;
    }

    // генерируем все остовные деревья
    vector<vector<int>> allMsts;
    vector<int> current;
    generateCombinations(edges, n, m, 0, current, allMsts);

    // минимальный вес (по алгоритму)
    int minWeight = findMinWeight(edges, allMsts);

    // выбираем только те, у которых вес минимальный
    vector<vector<int>> result = filterMsts(edges, allMsts, minWeight);

    // вывод результатов
    printResults(result, out);

    return 0;
}

// O(n + m log(n))
// unite -- log(n)
// getParent -- log(n)
bool isSpanningTree(const vector<Edge>& edges, int n, const vector<int>& subset) {
    // нам на вход поступает множество вершин, которое мы будем соеденять
    // алгоритм прост: выполняем unite в dsu и тем самым ищем циклы и отсутствие связанности
    DSU dsu(n);

    for (int idx : subset) {
        const Edge& e = edges[idx];
        if (!dsu.unite(e.u, e.v)) { 
            return false; 
        }
    }

    int root = dsu.getParent(1);
    for (int i = 2; i <= n; i++) {
        if (dsu.getParent(i) != root) {
            return false; 
        }
    }

    return true;
}


// реализована рекурсивно
// полный перебор C(m n-1)
void generateCombinations(const vector<Edge>& edges, int n, int m, int start,
                         vector<int>& current, vector<vector<int>>& allMsts) {
    if (current.size() == n - 1) {
        if (isSpanningTree(edges, n, current)) {
            allMsts.push_back(current);
        }
        return;
    }
    
    for (int i = start; i < m; i++) {
        current.push_back(i);
        generateCombinations(edges, n, m, i + 1, current, allMsts);
        current.pop_back();
    }
}

// полностью пересматривает O(C(m n-1))
int findMinWeight(const vector<Edge>& edges, const vector<vector<int>>& allMsts) {
    int minWeight = INT_MAX;
    for (const auto& mst : allMsts) {
        int weight = 0;
        for (int idx : mst) {
            weight += edges[idx].weight;
        }
        if (weight < minWeight) {
            minWeight = weight;
        }
    }
    return minWeight;
}

// в принципе как и findMinWeight
vector<vector<int>> filterMsts(const vector<Edge>& edges,
                             const vector<vector<int>>& allMsts, int minWeight) {
    vector<vector<int>> result;
    for (const auto& mst : allMsts) {
        int weight = 0;
        for (int idx : mst) {
            weight += edges[idx].weight;
        }
        if (weight == minWeight) {
            vector<int> ids;
            for (int idx : mst) {
                ids.push_back(edges[idx].id);
            }
            result.push_back(ids);
        }
    }
    return result;
}

void printResults(const vector<vector<int>>& result, ostream& out) {
    out << result.size() << endl;
    for (const auto& mst : result) {
        for (int id : mst) {
            out << id << " ";
        }
        out << endl;
    }
}