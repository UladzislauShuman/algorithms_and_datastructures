#include <fstream>
#include <vector>

class HashTable {
    private:
        std::vector<int> table;
        const int m;
        const int c;

        int hash(const int& x, const int& i) {
            return (x % m + c * i) % m;
        }
    public:
        HashTable(const int& m, const int& c) : m(m), c(c) {
            table = std::vector<int>(m,-1);
        }
        void add(const int& x) {
            int temp_index;
            for (int i = 0; i < m; ++i) {
                temp_index = hash(x,i);
                if (table[temp_index] == -1) { // нашли пустое место
                    table[temp_index] = x;
                    break;
                }
                if (table[temp_index] == x) { // уже есть -- не добавляем и дальше не продолжаем
                    break;
                }
            }
        }
        std::ofstream& show(std::ofstream& out) {
            for (const auto& element : table) {
                out << element << " ";
            }
            return out;
        }
};

int main() {
    std::ifstream in("input.txt");
    std::ofstream out("output.txt");

    int m,c,n;
    in >> m >> c >> n;
    HashTable hashTable(m,c);
    int temp_element;
    for (int i = 0; i < n; ++i) {
        in >> temp_element;
        hashTable.add(temp_element);
    }
    hashTable.show(out);
    
    in.close(); out.close();
    return 0;
}