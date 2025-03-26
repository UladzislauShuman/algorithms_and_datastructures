#include <fstream>
#include <vector>


int main() {
    std::ifstream in("input.txt");
    std::ofstream out("output.txt");

    int n,m; 
    in >> n >> m;
    std::vector<std::vector<int>> adjacency_matrix(n, std::vector<int>(n,0));

    int x,y; 
    for (int i = 0; i < m; ++i) {
        in >> x >> y;
        adjacency_matrix[x-1][y-1] = 1;
        adjacency_matrix[y-1][x-1] = 1;
    }

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            out << adjacency_matrix[i][j] << " ";
        }
        out << "\n";
    }
    
    in.close(); out.close();
    return 0;
}