#include <fstream>
#include <vector>


int main() {
    std::ifstream in("input.txt");
    std::ofstream out("output.txt");

    int n,m; in >> n >> m;

    std::vector<int> temp; temp.reserve(m);
    std::vector<std::vector<int>> graph(n, temp);
    
    int v1, v2;
    for (int i = 0; i < m; ++i) {
        in >> v1 >> v2; 
        graph[v1-1].push_back(v2);
        graph[v2-1].push_back(v1);   
    }

    for (int i = 0; i < n; ++i) {
        out << graph[i].size() << " ";
        for (int j = 0; j < graph[i].size(); ++j) {
            out << graph[i][j] << " ";
        }
        out << "\n";
    }
    
    in.close(); out.close();
    return 0;
}