#include <fstream>
#include <vector>
#include <algorithm>

/*
в данной задаче нам было предложено воспользоваться струкуторой данных DSU
и как это нам поможет в решении задачи?
рассмотрим на примере примерного теста
как я понял -- по началу мы имеем 
5 множеств, что не пересекаются
далее мы после каждого запроса как бы объеденяем множества
    но если две вершины имеют одинакового Главоря -- объеденения не будет

по началу у нас 5 компонент связанности (сколько вершин)
потом -- если мы произвели объеденение, то это зн -- -1 компонента связанности
иначе -- всё столько же компонент
*/

class DisjointSetUnionCity {
    private:
    struct City {
        int parent;
        int size;
        City(int parent, int size) : parent(parent) , size(size){}
    };
        /*
        index -- номер города
        Element.parent -- его родитель по DSU
        Element.size -- сколько ещё эл-ов на него повешано
        */
        std::vector<City> cities;
        int connected_components;
    public:
        DisjointSetUnionCity(const int size) : connected_components(size) {
            this->cities.reserve(size);
            for (int i = 0;i < size; ++i)
                this->cities.push_back(City(i+1, 1));
        }

        int findMainCity(const int city_number) { // на вход номер города
            if (city_number == this->cities[city_number-1].parent) // согласно реализации -- если номер города и родитель совпадают -- это Главарь
                return city_number; // которого мы и ищем
            this->cities[city_number-1].parent = this->findMainCity(this->cities[city_number-1].parent); // в противном случае мы сжимаем
            return this->cities[city_number-1].parent;
        }

        int makeRoad(const int city_number_1,const int city_number_2) {
            // находим Главарей множества
            int cityNumber1Parent = this->findMainCity(city_number_1); 
            int cityNumber2Parent = this->findMainCity(city_number_2); 

            if (cityNumber1Parent != cityNumber2Parent) {
                // далее мы хотим присоеденить меньшее по весу к большему
                if (this->cities[cityNumber1Parent-1].size < this->cities[cityNumber2Parent-1].size) // чтобы далее алгоритм работал как для всех
                    std::swap(cityNumber1Parent, cityNumber2Parent);
                this->cities[cityNumber2Parent-1].parent = cityNumber1Parent;
                this->cities[cityNumber1Parent-1].size += this->cities[cityNumber2Parent-1].size;
                return --this->connected_components;
            }
            // если же они уже в одном множестве == совпадают главари, то компонент связанности меньше и не стало -- ничего не изменилось
            return this->connected_components;
        }
};

int main() {
    std::ifstream in("input.txt");
    std::ofstream out("output.txt");

    int n,q; in >> n >> q;
    DisjointSetUnionCity cities(n);
    int from_, to_;
    for (int i = 0; i < q; ++i) {
        in >> from_ >> to_;
        out << cities.makeRoad(from_, to_) << "\n";
    }
    
    in.close(); out.close();
    return 0;
}