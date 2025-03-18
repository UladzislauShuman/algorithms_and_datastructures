#include <fstream>
#include <vector>
#include <algorithm>
#include <iostream>
#include <set>

struct  Tree
{
    unsigned int x;
    unsigned int y;
    Tree(const unsigned int x_, const unsigned int y_) {
        this->x = x_;
        this->y = y_;
    }
};

void getUniqueY(std::vector<int>& y, const std::vector<Tree>& trees, const unsigned int& high);
void getUniqueX(std::vector<int>& x, const std::vector<Tree>& trees, const unsigned int& width);
void setPark(std::vector<std::vector<int>>& park, const std::vector<Tree>& trees, const std::vector<int>& x);
long long getLargestPosiblePlaygroundArea(
    std::ifstream& in
) {
    unsigned int n, width, high;
    in >> n >> width >> high;
    std::vector<Tree> trees; trees.reserve(n);
    
    // считываем данные 
    unsigned int temp_x, temp_y;
    for (int i = 0; i < n; ++i) {
        in >> temp_x >> temp_y;
        // мы не будем вклуючать те точки, которые стоят на границе парка
        if (!(temp_x == 0 or temp_x == width or temp_y == 0 or temp_y == high)) 
            trees.push_back(Tree(temp_x, temp_y));
    }

    //тут мы уже можем учесть ситуацию
    // когда у нас все точки на границе <=> trees наш будет пустым
    if (trees.empty()) {
        // то мы можем просто вывести width * high
        return width * high;
    }
    

    //сортируем полученный массив деревьев по Y, ну и внутри Y по X
    std::sort(trees.begin(), trees.end(), [](const Tree& a, const Tree& b){
        if (a.y == b.y) {
            return a.x < b.x;
        } else {
            return a.y < b.y;
        }
    });


    // далее мы создаём два массива
    // один из них -- это уникальные Y
    std::vector<int> y; getUniqueY(y,trees, high);
    // уникальные X
    std::vector<int> x; getUniqueX(x,trees, width);

    std::vector<std::vector<int>> park(y.size() - 1); // без учёта того, что у нас там high
    setPark(park, trees, x);
    /*
    теперь, исходя из того, что мы имеем, мы будем делать обход следующим образом
        for по низу i
            while по current_x что в park(по current_down_y);
                for по верху j
                    мы в бутерброде i-j И мы смотрим на точку current_x
                    здесь мы фиксируем i,current_x И меняем только j
                    - тут у нас уже точно есть какие-то точки, из-за которых мы будем
                    должны сузить нашу область Лева и Права
                    - может возникнуть вопрос -- почему j - 1?
                    j - 1 
                        -1 отвечате за то, что мы смотрим уровень ниже
                    - зачем нам смотреть уровень ниже?
                        предположим мы i = 0; j = 2;
                        на 1 точно есть какие-то точки
                        предположим, что мы сузили left и right
                        посчитали (temp_square), перешли дальше -- 0 и 3
                            из предыдущей итерации мы выяслинили, что
                            left и right уже не расширишь -- иначе если расишрить, то будут точки внутри
                        и мы ещё раз сужаем
                        и считаем что нам надо (temp_square)
                    - зачем нам isWall?
                        названия я лучше не придумал
                        в чём суть -- isWall = true, если 
                            на одном из рассмотренных j есть tree : tree.x = current_x
                        isWall == true то мы знаем, что внутри нашей рассматрвиаемой области есть такая точка
                            и если она есть, то мы как бы не right - left * up - down
                            а считаем между right - current_x и curren_x - left
    */
    int current_down_y; int current_up_y;
    int left, right, current_x;
    long long temp_square, max_square = 0;
    bool isWall;
    
    for ( // перебираем низ
        int i = 0;
        i < y.size() - 1; // чтобы не рассматривать случай когда мы на high-high
        ++i
    ) {
        current_down_y = y[i];
        if (((high - current_down_y) * width) < max_square) break; // уже бессмысленно считать дальше 
            
        for (int k = 0; k < park[i].size(); ++k) {// перебираем вверх
            left = 0; right = width; // сужаем границы
            isWall = false;
            current_x = park[i][k];


            for (
                int j = i + 1;
                j < y.size();
                ++j
            ) {
                current_up_y = y[j];
                if (j == i + 1) { // тк, по сути, нет больше точек -- зн и не кому мешать
                    max_square  = std::max((long long)(width * (current_up_y - current_down_y)), max_square);
                } else {// между нами точки точно уже есть
                    for (int h = 0; h < park[j - 1].size(); ++h) { 
                        if (park[j - 1][h] < current_x)//самое узкое к current_x, чтобы лишних деревьев не попало
                            left = std::max(park[j - 1][h], left);
                        else if (park[j - 1][h] > current_x) // когда у нас уже точки рассматриваются правее
                            right = std::min(park[j - 1][h],right);
                        else // == , т.е. есть точка внутри
                            isWall = true;
                    }
                }
                /*
				почему именно самая правая слева От точки И самая левая справа от Точки?
					Это показыавет Насколько широко мы можем сделать площадку
					ибо иначе -- взять пошире, то уже будет точка внутри
						а это нам нельзя
				*/

                if (isWall) // то делим на две части и считаем по отедльности
                    max_square = std::max(
                        max_square,
                        (long long) std::max(   (current_up_y - current_down_y) * (current_x - left),
                                    (current_up_y - current_down_y) * (right - current_x))
                    );
                else { // иначе не надо
                    max_square = std::max( // потерял
                        max_square,
                        (long long) (current_up_y - current_down_y) * (right - left)
                    );
                }
            }
        }
    }
    return max_square;
}

void getUniqueY(std::vector<int>& y, const std::vector<Tree>& trees, const unsigned int& high) {
    // отличие
    // мы храним 0 и high уровни
    // а в массив trees мы их не считывали
    y.reserve(trees.size() + 2);
    y.push_back(0);
    // делаем следующее : 
    // тк у нас отсортированы по Y, то нам 
    // считываем первый уникальный, если не уникальный, то не записываем
    for (const auto& tree : trees) 
        if (y[y.size() - 1] != tree.y) // если мы видим отличный от последнего элемент
            y.push_back(tree.y); // то мы добавялем новое значение Y
    y.push_back(high);
}
void getUniqueX(std::vector<int>& x, const std::vector<Tree>& trees, const unsigned int& width) {
    std::set<int> result; // как задачу 0.0
    for (int i = 0; i < trees.size(); ++i) {
        result.insert(trees[i].x);
    }
    x.assign(result.begin(), result.end());
}
void setPark(std::vector<std::vector<int>>& park, const std::vector<Tree>& trees, const std::vector<int>& x) {
    int temp_y = trees[0].y;
    for (const auto& x_ : x)
        park[0].push_back(x_);
    int i = 1;
    for (const auto& tree : trees) {
        if (tree.y != temp_y) {
            temp_y = tree.y;
            i++;
        } 
        park[i].push_back(tree.x);
    }
}

int main() {
    std::ifstream in("input.txt");
    std::ofstream out("output.txt");

    out << getLargestPosiblePlaygroundArea(in);

    in.close(); out.close();
    return 0;
}