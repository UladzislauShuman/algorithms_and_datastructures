#include <fstream>
#include <vector>
#include <iostream>

        /*
        когда мы решали наивно, то мы сначало строили дерево
        ну и считали всё как нужно И потом считали как СУММА(Высота листа*массу листа)
        потом мы предположили, что длинна в битах закодированного текста есть 
        сумма всех элементов , кроме листьев
        Почему? -- посмотрим на пример из презентации
        вершина с значением 
        2 -- это сумма Г и Ё
        3 -- 2 + Ж = Г + Ё + Ж
        6 = 3 + И = Г + Ё + Ж + И

        и если сложить  2 + 3 + 6, то выйдет
        3Г + 3Ё + 2Ж + И
        3Г -- 3 это глубина Г, Г -- "масса Г"

        и т.о. -- складывая всё кроме листьев, раскрыв скобки и переставив как нужно
        мы посути и получаем наш наивный способ

        И теперь, вместо того чтобы делать кучу сложных манипуляций, можно обойтись
        двумя массивами, один из которых для наших букв, а во втором хранить те самые значение нелистьев
        */

typedef unsigned long long ULL;

ULL merge(int& i, std::vector<int>& p, int& j, std::vector<ULL>& H);
ULL getVectorSum(std::vector<ULL>& v);

int main() {
  //  std::ifstream in("input3.txt");
//    std::ofstream out("output.txt");
    std::ifstream in("huffman.in");
    std::ofstream out("huffman.out");

    int n; in >> n;
    std::vector<int> p(n,0);
    for (int i=0; i<n; ++i)
        in >> p[i];

    std::vector<ULL> H; H.reserve(n);
    //   p      H
    int i = 0, j = 0; ULL temp;
    while (i < n or j < n) {
        temp = merge(i,p,j,H);
        if (temp == -1) break;
        else H.push_back(temp);
    }

    //std::cout << H[H.size()-1] << "\n";
    out << getVectorSum(H);

    in.close(); out.close();
    return 0;
}

ULL merge(int& i, std::vector<int>& p, int& j, std::vector<ULL>& H)
{
    std::vector<ULL> twoTemp; twoTemp.reserve(2);
    while(i < p.size() && j < H.size() and twoTemp.size() < 2)
        if (p[i] <= H[j])
            twoTemp.push_back(p[i++]);
        else 
            twoTemp.push_back(H[j++]);
    
    while(i < p.size() and twoTemp.size() < 2)
        twoTemp.push_back(p[i++]);
    
    while(j < H.size() and twoTemp.size() < 2)
        twoTemp.push_back(H[j++]);

    if (twoTemp.size() != 2)
        return -1;
    return twoTemp[0] + twoTemp[1];
}

ULL getVectorSum(std::vector<ULL>& v) 
{
    ULL sum = 0;
    for (auto& element : v) 
        sum += element;
    return sum;
}
