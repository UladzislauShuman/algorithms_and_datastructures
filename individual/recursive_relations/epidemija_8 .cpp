#include <vector>
#include <algorithm>
#include <fstream>
#include <tuple>



int getMaxPlaceAB(
    std::vector<std::vector<std::vector<unsigned short int>>>& dp,
    const unsigned short int& a,
    const unsigned short int& b,
    const unsigned short int& p,
    const std::vector<std::tuple<unsigned short int, unsigned short int, unsigned short int>>& rooms
);


std::vector<unsigned short int> getAPath(
    const std::vector<std::vector<std::vector<unsigned short int>>>& dp,
    const unsigned short int& a,
    const unsigned short int& b,
    const unsigned short int& p,
    const std::vector<std::tuple<unsigned short int, unsigned short int, unsigned short int>>& rooms
);
int main() {

    std::ifstream in("input.txt");
    std::ofstream out("output.txt");
    unsigned short int a, b, p;
    in >> a >> b >> p;

    std::vector<std::tuple<unsigned short int, unsigned short int, unsigned short int>> rooms(p);
    for (int i = 0; i < p; ++i) 
        in >> std::get<0>(rooms[i]) >> std::get<1>(rooms[i]) >> std::get<2>(rooms[i]);

    // dp[i][a_out][b_out] - максимальное количество размещенных больных, начиная с палаты i (будто бы она первая)
    // при условии, что осталось a_out больных A и b_out больных B  (как бы первоначальная наша задача, просто входные данные "урезанные")
    std::vector<std::vector<std::vector<unsigned short int>>> dp(p + 1, std::vector<std::vector<unsigned short int>>(a + 1, std::vector<unsigned short int>(b + 1, -1)));
     // dp[i][a][b] - максимальное количество размещенных больных, начиная с палаты i И a и b число Оставшихся соответсвенно
    out << getMaxPlaceAB(dp, a, b, p, rooms) << "\n";
    
    if (dp[0][a][b] == (a + b) and (a+b) != 0) // всех разместили
    {
        std::vector<unsigned short int> path = getAPath(dp, a, b, p, rooms);
        for (unsigned short int j = 0; j < path.size(); ++j)
            out << path[j] << (j == path.size() - 1 ? "" : " ");
    }

    in.close();out.close();
    return 0;
}

int getMaxPlaceAB(
    std::vector<std::vector<std::vector<unsigned short int>>>& dp,
    const unsigned short int& a,
    const unsigned short int& b,
    const unsigned short int& p,
    const std::vector<std::tuple<unsigned short int, unsigned short int, unsigned short int>>& rooms
)
{
    // все больные размещены
    for (unsigned short int i = 0; i <= p; ++i) {
        dp[i][0][0] = 0;
    }

    // нет палат
    for (unsigned short int i = 0; i <= a; ++i) {
        for (unsigned short int j = 0; j <= b; ++j) {
            dp[p][i][j] = 0; 
        }
    }

    // Заполняем таблицу dp в обратном порядке (с конца)
    // a_left и b_left - оставшиеся больные типа A и B, которых нужно разместить
    // Если палата i не может быть размещена (нет достаточного числа больных),
    // то dp[i][a][b] = dp[i+1][a][b] (оставляем без изменения)
    // Если палата i может быть размещена, то
    // dp[i][a][b] = max(dp[i+1][a][b], dp[i+1][a-n][b-n] + n) (размещаем n больных)
    for (short int i = p - 1; i >= 0; --i) {
        for (unsigned short int a_out = 0; a_out <= a; ++a_out) {
            for (unsigned short int b_out = 0; b_out <= b; ++b_out) {

                unsigned short int n = std::get<0>(rooms[i]); // общая вместимость палаты
                unsigned short int a_in = std::get<1>(rooms[i]); // сколько в ней Уже больных A
                unsigned short int b_in = std::get<2>(rooms[i]); // сколько в ней Уже больных B

                dp[i][a_out][b_out] = 0;

                /*
                        мы знаем dp[i+1, p], хотим расширить до [i,p]
                */
                if (a_in > 0) { // есди там уже седят больные A, которых нельзя всё же переселить
                    unsigned short int can_place = a_out < n - a_in? a_out : (n - a_in); //std::min(a_out, n - a_in); // либо оставшихся меньше, либо их все же не меньше, чем осталось мест в комнате
                    dp[i][a_out][b_out] = dp[i+1][a_out - can_place][b_out] + can_place;
                    /*
                        случай, когда у нас нет вариантов И есть вариант только добавить
                        т.е., мы разместили на i, и теперь нас как бы инетересует [i+1,p]
                        т.е. максимальное количество размещенных больных, начиная с палаты i
                        Когда у нас в i-ой палате уже есть больные А
                        равно Количеству размещенных + нам осталось разместить a - current И b
                        и нас интересует их максимальное размещение (т.е. dp[i+1][a-current][b])
                    */
                } else if (b_in > 0) { // есди там уже седят больные B
                    unsigned short int can_place = b_out < n - b_in? b_out : (n - b_in);//std::min(b_out, n - b_in);
                    dp[i][a_out][b_out] = dp[i+1][a_out][b_out - can_place] + can_place;
                    // ситуация аналогичная описанной выше в A
                } else { // когда палата полностью пуста
                    /*
                        когда я решал рекурсивно, то эт как бы эквивалентно тому, что
                        мы "раскрываем" рекурсию И сравниваем две ветки (разместили больных А (влево) или разместили больных B (вправо)
                    */
                    unsigned short int place_a = (a_out >= n) ? dp[i+1][a_out - n][b_out] + n : dp[i+1][0][b_out] + a_out;
                    unsigned short int place_b = (b_out >= n) ? dp[i+1][a_out][b_out - n] + n : dp[i+1][a_out][0] + b_out;
                    /*
                        a_out >= n -- если false, то a_out - n -- отрицательное <=> закинули определённого типа больных И место осталось, что уже нам не важно
                    */
                    dp[i][a_out][b_out] = std::max(place_a, place_b);
                }
            }
        }
    }
    return dp[0][a][b]; // маскимальное число размещенных больных, начиная с первой палаты И надо разместить a и b
}

/*
    предпологаю, что я всё правльно, просто что неправльно вывожу

    а именно:
    тест :
    4
    0
    3
    4 1 0
    50 50 0
    50 47 0
     
    и я должен вывести
    4 
    1 2 3
    или (эта попытка на проверку вот этого)
    4 
    1 3
    ?
*/
std::vector<unsigned short int> getAPath(
    const std::vector<std::vector<std::vector<unsigned short int>>>& dp,
    const unsigned short int& a,
    const unsigned short int& b,
    const unsigned short int& p,
    const std::vector<std::tuple<unsigned short int, unsigned short int, unsigned short int>>& rooms
)
{
    // Восстановление пути (где размещены больные А)
    std::vector<unsigned short int> path; path.reserve(p);
    short int i = 0, a_out = a, b_out = b;
    while (i < p && (a_out >/*=*/ 0 || b_out >/*=*/ 0)) { 
    
        unsigned short int n = std::get<0>(rooms[i]); // общая вместимость палаты
        unsigned short int a_in = std::get<1>(rooms[i]); // сколько в ней Уже больных A
        unsigned short int b_in = std::get<2>(rooms[i]); // сколько в ней Уже больных B

        if (a_in > 0) { // есди там уже седят больные A, которых нельзя всё же переселить
            int can_place = a_out < n - a_in? a_out : (n - a_in); //std::min(a_out, n - a_in);
            if (can_place > 0){ // если == 0, то -- мы там как бы никого и не 
                a_out -= can_place;
                path.push_back(i + 1);
            } 
        } else if (b_in > 0) // есди там уже седят больные В, которых нельзя всё же переселить
            b_out -= b_out < n - b_in? b_out : (n - b_in); // ещё я заметил вот такой недочёт
        
        else { // палата пустая
            int place_a = (a_out >= n) ? dp[i+1][a_out - n][b_out] + n : dp[i+1][0][b_out] + a_out;
            int place_b = (b_out >= n) ? dp[i+1][a_out][b_out - n] + n : dp[i+1][a_out][0] + b_out;

            if(dp[i][a_out][b_out] == place_a)
            {
                path.push_back(i+1);
                    if (a_out >= n)
                        a_out -= n;
                    else
                        a_out = 0;
            }
            else if (dp[i][a_out][b_out] == place_b)
            {
                if (b_out >= n)
                    b_out -= n;
                else
                    b_out = 0;
            }    
        }   
        i++;
    }
    std::sort(path.begin(), path.end());
    return path;
}