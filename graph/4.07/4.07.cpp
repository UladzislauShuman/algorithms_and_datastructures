#include <fstream>
#include <vector>
#include <queue>
#include <limits>

const long long INF = std::numeric_limits<long long>::max();

using namespace std;

struct PairVW {
    int v;
    long long w;

    long long getW(){return w;}
    int getV(){return v;}

    PairVW(int v, long long w) {
        this->v = v;
        this->w = w;
    }

    PairVW() {
        this->v = 0;
        this->w = 0;
    }
};

struct Compare {
    bool operator()(const PairVW& a, const PairVW& b) const {
        return a.w > b.w; 
    }
};

int main() {
    std::ifstream in("input.txt");
    std::ofstream out("output.txt");

    int n, m; in >> n >> m;
    int temp_start, temp_end, temp_price;
    
    // реализуем списки смежности
    vector<vector<PairVW>> graph(n);

    for (int i = 0; i < m; ++i) {
        in >> temp_start >> temp_end >> temp_price;
        graph[temp_start - 1].push_back(PairVW(temp_end, temp_price));
        graph[temp_end - 1].push_back(PairVW(temp_start, temp_price));
    }

    /*
    1) считаем все вершины не обработанными и всем элементам dist присваиваем INF
    */
    vector<long long> dist(n, INF);
    vector<bool> processed(n,false); 

    //2) стартовой вершине пологаем приоритет 0
    dist[0] = 0;

    // 3)добавляем в приоритетную очередь (start,0)
    priority_queue<PairVW, vector<PairVW>, Compare> pq;
    pq.push(PairVW(1, dist[1 - 1])); // в приоритете более лёгкие
    //4) пока приоритетная очередь не станет пустой, выполняем следующие действия
    PairVW current_pair;
    while(!pq.empty())  {
        //удаляем элемент из приоритетной очереди
        current_pair = pq.top(); pq.pop();

        if (current_pair.getW() > dist[current_pair.getV() - 1]) {
            continue;
        }
        //если вершина была уже обработана, то возрващаемся обратно
        if (processed[current_pair.getV() - 1] == true) {
            continue;
        }
        //полагаем вершину обработанной
        processed[current_pair.getV() - 1] = true;
        dist[current_pair.getV() - 1] = current_pair.getW();
        // просматриваем вcе смежные с current и необработанные вершины и добавляем в приоритетную очередь
        for (int i = 0; i < graph[current_pair.getV() - 1].size(); ++i) {
            if (!processed[graph[current_pair.getV() - 1][i].getV() - 1] and 
            (dist[graph[current_pair.getV() - 1][i].getV() - 1] > dist[current_pair.getV() - 1] + graph[current_pair.getV() - 1][i].getW()) )
            {
                dist[graph[current_pair.getV() - 1][i].getV() - 1] = dist[current_pair.getV() - 1] + graph[current_pair.getV() - 1][i].getW();
                pq.push(PairVW(
                    graph[current_pair.getV() - 1][i].getV(),
                    dist[current_pair.getV() - 1] + graph[current_pair.getV() - 1][i].getW()
                ));
            }
        }
    }

    //в результате имеем
    out << dist[n - 1];

    in.close(); out.close();
    return 0;
}