#include <fstream>
#include <vector>
#include <algorithm>

using namespace std;

int main() {
    std::ifstream in("input1.txt");
    std::ofstream out("output.txt");

    int n, m; in >> n >> m;
    int temp_start, temp_end, temp_price;
    
    vector<vector<int>> graph(n, vector<int>(n,0));

    for (int i = 0; i < m; ++i) {
        in >> temp_start >> temp_end >> temp_price;
        // 3 тест показывает то, что -- у нас как бы может быть и повторки
        // но не имеет смысл их хранить, тк обязательно пойдут по минимальной
        graph[temp_start - 1][temp_end - 1] = 
             graph[temp_end - 1][temp_start - 1] = min(graph[temp_start - 1][temp_end - 1], temp_price);
    }



    in.close(); out.close();
    return 0;
}