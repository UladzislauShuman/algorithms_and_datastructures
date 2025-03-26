#include <fstream>
#include <vector>


int main() {
    std::ifstream in("input.txt");
    std::ofstream out("output.txt");

    int n; in >> n;
    std::vector<int> graph(n, 0);
    int v1, v2;
    for (int i = 0; i < n - 1; ++i) {
        in >> v1 >> v2;
        graph[v2-1] = v1;
    }
    
    for(int i = 0; i < n; ++i) {
        out << graph[i] << " ";
    }
    
    in.close(); out.close();
    return 0;
}