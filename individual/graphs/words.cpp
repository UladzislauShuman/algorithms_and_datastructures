#include <fstream>
#include <vector>
#include <string>
#include <unordered_map>

/*
идея состоит в том, чтобы 
матрица 26х26 -- сколько букв в английском лацинском алфавите
мы рассмотрим ориентированный граф, где 
начало слова --> конец слова
его рёбра  -- слова
ну если слова будут повторяться, то это будет массив из слов
*/
int main() {
    std::ifstream in("input.txt");
    std::ofstream out("output.txt");

    in >> n;
    std::vector<std::string> words(n);
    std::unordered_map<char, std::unordered_map<char, std::vector<string>>> graph;

    for (int i = 0; i < n; ++i) {
        in >> words[i];
        char start = words[i].front();
        char end = words[i].back();
        graph[start][end].push_back(words[i]);
    }

    
    in.close(); out.close();
    return 0;
}