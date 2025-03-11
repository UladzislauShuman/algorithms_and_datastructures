#include <fstream>
#include <vector>
#include <algorithm>
#include <iostream>
#include <string>
/*
связанный граф -- граф, что имеет одну компоненту связанности
в нашем случае это эквивалентно тому, что у нас только одно множество в СНМ
т.е. -- если в какой-то момент у нас получилось 2 множества в результате удаления дороги
то граф у нас уже не связанный

из прошлой задачи, мой метод makeRoad -- возвращает количество компонентов связанности
т.е., если построить оставшиеся дороги, то -- получиться результат

далее, у нас есть дороги в порядке удаления
мы пойдём с конца И будем их добавлять в Дороги
т.е. -- мы пришли в конец истории и крутим время вспять, дороги собираются 

и далее makeRoad в return говорит сколько компонент связанности
и ведь -- мы можем на определённом Восстановлении дорог в обратном порядке
прийти к тому, что метод возвращает 1 -- т.е. одна компонента связанности
те города связаны
ну и технчиески, после этого момента, как не добавляй
все равно будет связанный

т.е. -- до первого момента, пока при прокрутке времени вспять, пока makeRoad 
не вернул 1, мы пишем 0 и Восстанавливаем дороги
а когда первый момент наступил -- ставим 1 и далее все остальные тоже 1
*/

class DisjointSetUnionCity {
    private:
    struct City {
        int parent;
        int size;
        City(int parent, int size) : parent(parent) , size(size){}
    };
    struct Road {
        int begin;
        int end;
        bool isDeleted;
        Road(int begin , int end) : begin(begin), end(end), isDeleted(false){}
        bool isNullRoad(const Road& road) {
            return road.begin == -1 and road.end == -1;
        }
        void setRoad(const Road& road) {
            this->begin = road.begin;
            this->end = road.end;
        }
    };
        /*
        index -- номер города
        Element.parent -- его родитель по DSU
        Element.size -- сколько ещё эл-ов на него повешано
        */
        std::vector<City> cities;
        std::vector<Road> roads;
        std::vector<int> deleted_roads;

        int connected_components;
    public:
        DisjointSetUnionCity(const int size) : connected_components(size) {
            this->cities.reserve(size);
            for (int i = 0;i < size; ++i)
                this->cities.push_back(City(i+1, 1));
        }
        DisjointSetUnionCity(const int size,const int roads_amount, const int deleted_roads_amount) : DisjointSetUnionCity(size) {
            this->roads.reserve(roads_amount);
            this->deleted_roads.reserve(deleted_roads_amount);
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
        int makeRoad(const Road& road) {
            return makeRoad(road.begin, road.end);
        }

        void addRoad(const int begin, const int end) {
            this->roads.push_back(Road(begin, end));
        }
        void addIndexOfDeletedRoad(const int indexOfDeletedRoad) {
            this->deleted_roads.push_back(indexOfDeletedRoad - 1);
        }

        void deleteAllDeletedRoads() {
            for (int i = 0; i < this->deleted_roads.size(); ++i)
                this->roads[this->deleted_roads[i]].isDeleted = true;
        }

        void buildFromExists() {
            for (int i = 0; i < this->roads.size(); ++i){
                if(!this->roads[i].isDeleted)
                    this->makeRoad(this->roads[i]);
            }
        }

        void result(std::ofstream& out) {
            std::vector<bool> result_;
            result_.reserve(this->deleted_roads.size());
            int i;
            for (i = this->deleted_roads.size()-1; i >= 0; --i) {
                if(this->connected_components == 1) {
                    break;
                } else {
                    result_.push_back(false);
                }  
                //
                std::cout << this->connected_components << std::endl;
                //
                this->makeRoad(this->roads[this->deleted_roads[i]]);    
            }
            while (i >= 0) {
                result_.push_back(true);
                --i;
            }

            for (auto it = result_.rbegin(); it != result_.rend(); ++it)
                out << *it;
        }

};

int main() {
    std::ifstream in("input.txt");
    std::ofstream out("output.txt");

    //считываем данные
    int n,m,q; in >> n >> m >> q;
    DisjointSetUnionCity cities(n,m,q);
    int from_, to_;
    for (int i = 0; i < m; ++i) {
        in >> from_ >> to_;
        cities.addRoad(from_,to_);
    }

    int road_number;
    for (int i = 0; i < q; ++i) {
        in >> road_number;
        cities.addIndexOfDeletedRoad(road_number); // ответсвенность за 0-индексацию будет на методе
    }
    
    // удаляем все дороги из массива (просто меняем на маркеры)
    cities.deleteAllDeletedRoads();
    // строим дороги из оставшихся 
    cities.buildFromExists();
    // решаем задачу
    cities.result(out);

    in.close(); out.close();
    return 0;
}