/*
нам нужно найти все минимальные остовные деревья

минимальное остовное дерево -- это минимальное по весу остовное дерево в графе
^
остовновное дерево -- это остовный подграф, который сам является деревом
^
остовный подграф -- это подграф графа, и у этих графов одинаковое множество вершин

тем самым нам нужно
- найти вес минимального остовного дерева
- относительно Этого веса построить все такие остовные деревья, вес которых равен Этому весу

отсечения и оптимизации полного перебора
- определить минимальный вес по алгоритму Крускало O(M logM [сортировка ребер] + M logN[использую DSU с оптимизацией])
- отсечение по весу (если уже превышает минимальный вес -- отсекаем)
    - бывают случаи, когда все ребра имеют одинаковый вес
        тогда отсечений не будет по этому признаку
    - в случае если веса разныве, то можно получить приличный результат
- отсечение по связанности -- при добавлении получается цикл? -- отсекаем
    - используем DSU для проверки связанности и цикличности
*/


#include <fstream>
#include <vector>
#include <algorithm>
#include <iostream>

using namespace std;

struct Edge
{
    int i;
    int j;
    int d;
    int id; // я планирую сортировать, поэтому пригодиться запомнить

    Edge(): i(0), j(0), d(0), id(0){}
    Edge(int i, int j, int d, int id): i(i), j(j), d(d), id(id){}
};

struct DSU 
{
    vector<int> parents;
    vector<int> rank; 
    vector<pair<int, int>> history_parents;
    vector<pair<int, int>> history_rank;

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
        
        history_parents.push_back({parent_b, parents[parent_b]});
        history_rank.push_back({parent_a, rank[parent_a]});
        parents[parent_b] = parent_a;
        if (rank[parent_a] == rank[parent_b]) {
            rank[parent_a]++;
        }
        return true;
    }

    void doCtrlS() {
        history_parents.emplace_back(-1, -1);
        history_rank.emplace_back(-1, -1);
    }

    void doCtrlZ() {

        while (!history_parents.empty()) {
            auto [v, prev_parent] = history_parents.back();
            history_parents.pop_back();
            if (v == -1) break; 
            parents[v] = prev_parent;
        }
        while (!history_rank.empty()) {
            auto [v, prev_rank] = history_rank.back();
            history_rank.pop_back();
            if (v == -1) break; 
            rank[v] = prev_rank;
        }
    }
};

long long getMSTWeight(vector<Edge> graph, const int n,const int m);
void backtrack(int start, int current_weight, DSU& dsu, 
    const int n, const long long& MSTWeight, const vector<Edge>& graph,
    vector<int>& current_graph, vector<vector<int>>& allMST);
vector<vector<int>> getAllMST( 
        const int n, const long long& MSTWeight, vector<Edge> graph);

int main() {
    ifstream in("test2.txt");
    //ifstream in("input.txt");
    ofstream out("output.txt");

    // считываем данные
    int n,m;
    in >> n >> m;

    vector<Edge> graph; graph.reserve(m);
    int temp_i, temp_j, temp_d;
    for (int i = 0; i < m; ++i) {
        in >> temp_i >> temp_j >> temp_d;
        graph.push_back(Edge(temp_i, temp_j, temp_d, i + 1));
    }

    long long MSTWeight = getMSTWeight(graph, n ,m);
    //cout << "MSTWeight = " << MSTWeight << "\n";
    vector<vector<int>> allMST = getAllMST(n, MSTWeight, graph);

    //выводим
    out << allMST.size() << endl;
    for (const vector<int>& mst : allMST) {
        for (int i = 0; i < mst.size() - 1; ++i) {
            out << mst[i] << " ";
        }
        out << mst[mst.size() - 1] << "\n";    
    }

    in.close(); out.close();
    return 0;
}

// алгоритм Крускало
long long getMSTWeight(vector<Edge> graph, const int n,const int m) {
    
    //сортируем ребра согласно их весу
    sort(graph.begin(), graph.end(), [](const Edge& a, const Edge& b){
        return a.d < b.d;
    });
    DSU dsu(n);
    long long weight = 0;
    
    for (Edge edge : graph) 
        if (dsu.unite(edge.i, edge.j)) 
            weight += edge.d;

    return weight;
}


vector<vector<int>> getAllMST(const int n, const long long& MSTWeight, vector<Edge> graph) {
    sort(graph.begin(), graph.end(), [](const Edge& a, const Edge& b) {
        return  (a.d < b.d);
    });

    vector<int> current_graph;
    vector<vector<int>> allMST;
    DSU dsu(n);

    for (int start = 0; start < graph.size(); ++start) {
        backtrack(start, 0, dsu, n, MSTWeight, graph, current_graph, allMST);
    }

    // удаляем дубликаты
    sort(allMST.begin(), allMST.end()); // unique после сортировки лучше работает
    allMST.erase(unique(allMST.begin(), allMST.end()), allMST.end());

    return allMST;
}

// рекурсивно
void backtrack(int start, int current_weight, DSU& dsu, 
    const int n, const long long& MSTWeight, const vector<Edge>& graph,
    vector<int>& current_graph, vector<vector<int>>& allMST) {
        if (current_graph.size() == n - 1) {
            if (current_weight == MSTWeight) 
                allMST.push_back(current_graph);
            return;
        }

    for (int i = start; i < graph.size(); ++i) {
        if (current_weight + graph[i].d > MSTWeight) 
            continue;
        

        dsu.doCtrlS(); // сохраняем
        // изменяем
        if (dsu.unite(graph[i].i, graph[i].j)) {
            current_graph.push_back(graph[i].id);
            backtrack(i + 1, current_weight + graph[i].d, dsu, 
                n, MSTWeight, graph,
                current_graph, allMST
            );
            current_graph.pop_back();
        }
        dsu.doCtrlZ(); // возвращаем
    }
}