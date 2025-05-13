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

но -- это не достаточно эффективно
на паре Владимир Михайлович рассказал о куда более интеренсом и быстром подходе 
идея такая
    - строим первое MST по какому-либо алгоритму
    - и далее, на его основе строим все остальные
        все ребра у нас отсортированы по весу
        у нас уже есть Рекорд (первое MST)
        мы на какой-то итерации и пытаемся добавить ребро
        если при добавлении Мы уже имеем цикл, то
            у нас либо веса ребер в цикле по отдельности меньше чем вес новенького
            либо они равны
            и вот если они равны
                то мы просто по разному комбинируем и получаем разные MST
                которые и заносим в allMST

в этой попытке я пробую увеличить стек И краевой случай обработал
n = 1 m = 0
*/

#include <fstream>
#include <vector>
#include <algorithm>
#include <iostream>
// для увеличения стека с win
// #include <windows.h>

// void increase_stack() {
//     ULONG_PTR size = 128 * 1024 * 1024;
//     SetThreadStackGuarantee((PULONG)&size);
// }

// увеличение стека на macOs/Linux
// #include <sys/resource.h>
// void increase_stack() {
//     const rlim_t stack_size = 128 * 1024 * 1024; // 128 MB
//     struct rlimit rl;
    
//     // Получаем текущие лимиты
//     if (getrlimit(RLIMIT_STACK, &rl) == 0) {
//         // Если текущий лимит меньше желаемого
//         if (rl.rlim_cur < stack_size) {
//             rl.rlim_cur = stack_size; // Устанавливаем новый мягкий лимит
//             // Попробуем установить жесткий лимит тоже (но не все системы позволяют)
//             if (rl.rlim_max < stack_size) {
//                 rl.rlim_max = stack_size;
//             }
//             setrlimit(RLIMIT_STACK, &rl);
//         }
//     }
// }

using namespace std;

struct Edge
{
    int i;     
    int j;      
    int d;      
    int id;    

    Edge(): i(0), j(0), d(0), id(0){}
    Edge(int i, int j, int d, int id): i(i), j(j), d(d), id(id){}
};

struct DSU 
{
    vector<int> parents;

    DSU(int n) {
        parents.resize(n + 1);
        for (int i = 1; i <= n; ++i) {
            parents[i] = i;
        }
    }

    int getParent(int vertex) {
        int parent = vertex;
        while (parent != parents[parent]) {
            parent = parents[parent];
        }
        while (vertex != parent) {
            int next = parents[vertex];
            parents[vertex] = parent;
            vertex = next;
        }
        return parent;
    }

    bool unite(int a, int b) {
        int parent_a = getParent(a);
        int parent_b = getParent(b);
        if (parent_a == parent_b) {
            return false;
        } else {
            parents[parent_a] = parent_b;
            return true;
        }
    }
};

long long getMSTWeight(vector<Edge>& graph, int n);
bool buildMST(const vector<int>& includedEdges, const vector<int>& excludedEdges, 
            const vector<Edge>& graph, vector<vector<int>>& allMSTs,
            int n, int m, long long MSTWeight, vector<int>& currentEdges);
void getAllMSTs_(vector<int> includedEdges, vector<int> excludedEdges, 
               vector<int>& currentEdges, const vector<Edge>& graph, 
               vector<vector<int>>& allMSTs, int n, int m, long long MSTWeight);
vector<vector<int>> getAllMSTs(int n, int m, long long MSTWeight, const vector<Edge>& graph);

int main() {
    //increase_stack();
    ifstream in("input.txt");
    ofstream out("output.txt");

    // input
    int n, m;
    in >> n >> m;
    vector<Edge> graph;
    graph.reserve(m);
    for (int i = 0; i < m; ++i) {
        int a, b, c;
        in >> a >> b >> c;
        graph.emplace_back(a, b, c, i + 1);
    }

    // вес MST
    long long MSTWeight = getMSTWeight(graph, n);
    // все MST
    vector<vector<int>> allMSTs = getAllMSTs(n, m, MSTWeight, graph);

    // output
    out << allMSTs.size() << endl;
    if (allMSTs.size() == 1 && allMSTs[0].empty()) {
        out << " " << endl;
    } else {
        for (const auto& mst : allMSTs) {
            for (int i = 0; i < mst.size() - 1; ++i) {
                out << mst[i] << " ";
            }
            out << mst[mst.size() - 1] << "\n";
        }
    }
    
    in.close(); out.close();
    return 0;
}

// реализованно с помощью алгоритма Крускала
long long getMSTWeight(vector<Edge>& graph, int n) {
    // сортируем по весу
    sort(graph.begin(), graph.end(), [](const Edge& a, const Edge& b){
        return a.d < b.d;
    });
    
    // используем DSU
    DSU dsu(n);
    long long totalWeight = 0;
    
    for (const Edge& edge : graph) {
        if (dsu.unite(edge.i, edge.j)) { // если не образует цикл
            totalWeight += edge.d;
        }
    }

    return totalWeight;
}

vector<vector<int>> getAllMSTs(int n, int m, long long MSTWeight, const vector<Edge>& graph) {
    vector<vector<int>> allMSTs;
    
    if (n == 1) {
        allMSTs.push_back(vector<int>());
        return allMSTs;
    }

    vector<int> includedEdges; // ребра, что будут обязательно входить в MST
    vector<int> excludedEdges(m, 0); // эти ребра будет запрещено использовать
    // пока они ничего в себя не включают -- мы даже не начали искать MST
    vector<int> currentEdges; // те, с которым мы сейчас работаем
    
    // строим первое (так сказать -- Рекорд)
    if (buildMST(includedEdges, excludedEdges, 
        graph, allMSTs, n, m, MSTWeight, currentEdges)) {
        // и все остальные на его основе
        getAllMSTs_(includedEdges, excludedEdges, currentEdges,
             graph, allMSTs, n, m, MSTWeight);
    }

    return allMSTs;
}

// тоже алгоритм Крускала
bool buildMST(const vector<int>& includedEdges, const vector<int>& excludedEdges, 
            const vector<Edge>& graph, vector<vector<int>>& allMSTs,
            int n, int m, long long MSTWeight, vector<int>& currentEdges) {
    int totalWeight = 0;
    currentEdges.clear();
    int edgesCount = 0;
    DSU dsu(n);

    // добавляем обязательные ребра
    for (int idx : includedEdges) {
        edgesCount++;
        dsu.unite(graph[idx].i, graph[idx].j);
        totalWeight += graph[idx].d;
    }

    // остальные
    for (int i = 0; i < m; ++i) {
        // если не запрещенные и так же разные родители
        if (!excludedEdges[i] && dsu.getParent(graph[i].i) != dsu.getParent(graph[i].j)) {
            totalWeight += graph[i].d;
            currentEdges.push_back(i);
            dsu.unite(graph[i].i, graph[i].j);
            edgesCount++;
        }
    }

    // если уже дерево И текущий вес == MSTWeight
    if (edgesCount == n - 1 && totalWeight == MSTWeight) {
        vector<int> currentMST;
        // добавляем обязательные ребера
        for (int idx : includedEdges) { // то добавляем те, что были обязательно
            currentMST.push_back(graph[idx].id);
        }
        // добавляем остальные ребра 
        for (int idx : currentEdges) { // и те, что мы высчитали
            currentMST.push_back(graph[idx].id);
        }
        allMSTs.push_back(currentMST); // и заносим ответ ко Всем
        return true;
    }
    return false;
} 

// рекурсивно
void getAllMSTs_(vector<int> includedEdges, vector<int> excludedEdges, 
               vector<int>& currentEdges, const vector<Edge>& graph, 
               vector<vector<int>>& allMSTs, int n, int m, long long MSTWeight) {
    vector<int> candidateEdges(currentEdges);
    
    for (int i = 0; i < candidateEdges.size(); ++i) {
        // на i-ой итерации запрещаем i-ое ребро
        if (i > 0) {
            excludedEdges[candidateEdges[i - 1]] = 0;// разрешаем предыдущее
            includedEdges.push_back(candidateEdges[i - 1]);// добавляем в обязательные
        }
        excludedEdges[candidateEdges[i]] = 1;// Запрещаем текущее ребро
        
        // Пытаемся построить MST с новыми ограничениями
        if (buildMST(includedEdges, excludedEdges, graph, allMSTs, n, m, MSTWeight, currentEdges)) {
            getAllMSTs_(includedEdges, excludedEdges, currentEdges, graph, allMSTs, n, m, MSTWeight);
        }
    }
}