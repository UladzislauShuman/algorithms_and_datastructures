#include <fstream>
#include <vector>
    
// в матрице смежности для ориентированного графа у нас [i][j] от i к j
 
int main() {
    std::ifstream in("input.txt");
    std::ofstream out("output.txt");

    int n; in >> n;
    
    std::vector<int> result(n,0);
    int temp;

    for(int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            in >> temp;
            if (temp == 1)
                result[j] = i + 1;
        }
    }
    
    for (int i  = 0; i < n; ++i)
        out << result[i] << " ";

    in.close(); out.close();
    return 0;
}