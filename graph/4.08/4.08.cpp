#include <iostream>
#include <vector>
#include <algorithm>
#include <climits>

using namespace std;

struct Edge { // ребро
    int end;// конец ребра
    int price;// пропускная способнотсь
    int flow;// поток
    int pred_edge;// предшевствующее ребро
    
    Edge(int end, int price, int pred_edge)
        : end(end), price(price), flow(0), pred_edge(pred_edge) {}
};

class Network {
private:
    int n;
    vector<vector<Edge>> graph;// граф(список смежности)
    vector<bool> visited;// тут будем хранить тех, кого посетили
    vector<int> pred;// предки (будем востанавливать)
    
    bool dfs(int u, int stock) {
        if (u == stock) return true;
        visited[u] = true;
        
        for (Edge& edge : graph[u]) {
            if ( // если 
                visited[edge.end] == false && // мы след вершина не посещена
                edge.price > edge.flow // И пропускная способность ребра позволяет нам поток вместить
            ) {
                pred[edge.end] = u; // запоминаем 
                if (dfs(edge.end, stock) == true) { // и идём дальше
                    return true;
                }
            }
        }
        return false;
    }
    
    // Найти минимальную остаточную пропускную способность на пути
    int findMinResidual(int source, int stock) {
        int min_residual = INT_MAX;
        // проходим всех предков и ищем минимальное значение
        for (int vertex = stock; vertex != source; vertex = pred[vertex]) {
            int u = pred[vertex];
            for (Edge& edge : graph[u]) {// находим нужное нам ребро, пройдясь по всем
                if (edge.end == vertex && edge.price > edge.flow) {
                    min_residual = min(min_residual, edge.price - edge.flow);
                    break;
                }
            }
        }
        return min_residual;
    }
    
    // увеличивает поток вдоль пути на величину flow
    void increasePath(int source, int stock, int flow) {
        for (int vertex = stock; vertex != source; vertex = pred[vertex]) {
            int u = pred[vertex];
            for (Edge& edge : graph[u]) {
                if (edge.end == vertex && edge.price > edge.flow) {
                    edge.flow += flow;
                    graph[edge.end][edge.pred_edge].flow -= flow;// уменьшаем поток в обратном ребре
                    break;
                }
            }
        }
    }

public:
    Network(int n_) : n(n_) {
        graph.resize(n);
        visited.resize(n);
        pred.resize(n);
    }
    
    void addEdge(int u, int vertex, int capacity) {
        graph[u].emplace_back(vertex, capacity, graph[vertex].size());
        graph[vertex].emplace_back(u, 0, graph[u].size() - 1);
    }
    
    int getMaxFlow(int source, int stock) {
        if (source == stock) return 0; // если одно и тоже, то -- нет уже и смысла проверять
        
        int total_flow = 0;
        
        while (true) {
            for (int i = 0; i < visited.size(); ++i) 
                visited[i] = false;
            for (int i = 0; i < pred.size(); ++i) 
                pred[i] = -1;
            
            if (dfs(source, stock) == false) // не нашёл как пройти от Источника к Стоку
                break;
            
            int path_flow = findMinResidual(source, stock);
            increasePath(source, stock, path_flow);
            total_flow += path_flow;
        }
        
        return total_flow;
    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n, m;
    cin >> n >> m;
    
    Network network(n);
    
    int temp_u, temp_v, temp_price;
    for (int i = 0; i < m; ++i) {
        cin >> temp_u >> temp_v >> temp_price;
        network.addEdge(temp_u - 1, temp_v - 1, temp_price);
    }
    
    cout << network.getMaxFlow(0, n - 1);
    return 0;
}