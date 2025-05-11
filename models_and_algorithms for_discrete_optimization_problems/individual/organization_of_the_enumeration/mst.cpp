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

данную задачу я хочу решить по идее backtracing
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
    vector<pair<int, int>> history;

    DSU(int n) { // задает dsu из n вершин
        parents.resize(n + 1); // тк вершины нумеруются от 1 до n, то для удобства размер будет n+1
        rank.resize(n + 1, 1);
        for (int i = 1; i <= n; ++i) {
            parents[i] = i;
        }
    }

    int getParent(int v) { // сжатие + не рекурсивная 
        int parent = v;
        //нашли родителя
        while (parent != parents[parent]) {
            parent = parents[parent];
        }
        // сжали
        while (v != parent) {
            int next = parents[v];
            parents[v] = parent;
            v = next;
        }
        return parent;
    }

    bool unite(int a, int b) { // если будет false, то значит при объединении будет цикл
        int parent_a = getParent(a);
        int parent_b = getParent(b);
        if (parent_a == parent_b) {
            return false; 
        }
       
        if (rank[parent_a] < rank[parent_b]) {
            swap(parent_a, parent_b);
        }
        history.push_back({b, parents[parent_b]});  // запоминаем parents[b] = a
        parents[parent_b] = parent_a;
        if (rank[parent_a] == rank[parent_b]) {
            rank[parent_a]++;
        }
        return true;
    }

    void doCtrlZ() { // операция для отмены последней операции
        if (history.empty()) return;
        auto [v, prev_parent] = history.back();
        history.pop_back();
        parents[v] = prev_parent;
    }
};

long long getMSTWeight(vector<Edge> graph, const int n,const int m);
void backtrack(int start, int current_weight, DSU& dsu, 
    const int n, const long long& MSTWeight, const vector<Edge>& graph,
    vector<int>& current_graph, vector<vector<int>>& allMST);
vector<vector<int>> getAllMST( 
        const int n, const long long& MSTWeight, vector<Edge> graph);

int main() {
    ifstream in("test1.txt");
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
    vector<vector<int>> allMST = getAllMST(n, MSTWeight, graph);

    //выводим
    cout << allMST.size() << endl;
    for (const vector<int>& mst : allMST) {
        for (const int& edge_id : mst) 
           cout << edge_id << " ";
        cout << "\n";    
    }

    in.close(); out.close();
    return 0;
}

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
    sort(graph.begin(), graph.end(), [](const Edge& a, const Edge& b){
        if (a.d == b.d) 
            return a.id < b.id;
        return a.d < b.d;
    });
    
    vector<int> current_graph;
    vector<vector<int>> allMST;
    DSU dsu(n);
    
    // перебираем все старты
    for (int start = 0; start < graph.size(); ++start) {
        backtrack(start, 0, dsu, n, MSTWeight, graph, current_graph, allMST);
    }
    
    sort(allMST.begin(), allMST.end());
    allMST.erase(unique(allMST.begin(), allMST.end()), allMST.end());
    
    return allMST;
}

void backtrack(int start, int current_weight, DSU& dsu, 
    const int n, const long long& MSTWeight, const vector<Edge>& graph,
    vector<int>& current_graph, vector<vector<int>>& allMST) {
    if (current_graph.size() == n - 1) { // если уже дерево
        // мы получили остово дерево
        if (current_weight == MSTWeight) { // если обладает минимальным весом
            vector<int> sorted_mst = current_graph;
            sort(sorted_mst.begin(), sorted_mst.end());
            allMST.push_back(sorted_mst);
        }
        return; // иначе не интересно и мы закончили в этой ветке
    }

    for (int i = start; i < graph.size(); ++i) {
        if (current_weight + graph[i].d > MSTWeight) { // если уже вес больше, то смысла нет -- отсекаем ветку
            continue;
        }
        
        if (dsu.unite(graph[i].i, graph[i].j)) {
            current_graph.push_back(graph[i].id);
            backtrack(i + 1, current_weight + graph[i].d, dsu,
                n, MSTWeight, graph,
                current_graph, allMST
            );
            current_graph.pop_back();
            dsu.doCtrlZ();
        }
    }
}