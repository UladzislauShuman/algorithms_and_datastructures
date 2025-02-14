#include <fstream>
#include <vector>


int main() {
    std::ifstream in("input.txt");
    std::ofstream out("output.txt");

    
    in.close(); out.close();
    return 0;
}