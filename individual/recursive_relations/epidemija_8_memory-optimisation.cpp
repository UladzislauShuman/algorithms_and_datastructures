#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <tuple>

/*
я превысил лимит по памяти и предположил, что можно попробовать уменьшить
dp-массив И использовать только "две строки" 
тк из кода предыдущей попытки я заметил, что будуче на i, я обращаюсь только к i-1

но пригодиться сохранить пути обходов (разместили в комнате i A или B)
что будет тоже 3Д, но char -- который занимает 1 байт, что ещё меньше, чем unsigned short
*/

int getMaxPlaceAB(
    std::vector<std::vector<unsigned short int>>& dp_curr,
    std::vector<std::vector<unsigned short int>>& dp_next,
    std::vector<std::vector<std::vector<char>>>& choice,
    unsigned short int a, // кроме этого, я вспомнил, что адрес занимает 4 байта, а unsigned short только 2 байта
    unsigned short int b,
    unsigned short int p,
    const std::vector<std::tuple<unsigned short int, unsigned short int, unsigned short int>>& rooms
);
std::vector<unsigned short int> getAPath(
    const std::vector<std::vector<unsigned short int>>& dp_curr,
    const std::vector<std::vector<unsigned short int>>& dp_next,
    const std::vector<std::vector<std::vector<char>>>& choice,
    unsigned short int a,
    unsigned short int b,
    unsigned short int p,
    const std::vector<std::tuple<unsigned short int, unsigned short int, unsigned short int>>& rooms
);

int main() {
    std::ifstream in("input_max.txt");
    std::ofstream out("output.txt");

    unsigned short int a, b, p;
    in >> a >> b >> p;

    std::vector<std::tuple<unsigned short int, unsigned short int, unsigned short int>> rooms(p);
    for (int i = 0; i < p; ++i) 
        in >> std::get<0>(rooms[i]) >> std::get<1>(rooms[i]) >> std::get<2>(rooms[i]);

    std::vector<std::vector<unsigned short int>> dp_curr(a + 1, std::vector<unsigned short int>(b + 1, 0)); // i
    std::vector<std::vector<unsigned short int>> dp_next(a + 1, std::vector<unsigned short int>(b + 1, 0)); // i+1
    std::vector<std::vector<std::vector<char>>> choice(p, std::vector<std::vector<char>>(a + 1, std::vector<char>(b + 1, ' ')));

    int result = getMaxPlaceAB(dp_curr, dp_next, choice, a, b, p, rooms);
    out << result << "\n";
    
    if(result == (a + b) && (a + b) != 0) {
        std::vector<unsigned short int> path = getAPath(dp_curr, dp_next, choice, a, b, p, rooms);
        for (size_t i = 0; i < path.size(); ++i) {
            out << path[i] << (i + 1 < path.size() ? " " : "");
        }
    }
    
    in.close(); out.close();
    return 0;
}

int getMaxPlaceAB(
    std::vector<std::vector<unsigned short int>>& dp_curr,
    std::vector<std::vector<unsigned short int>>& dp_next,
    std::vector<std::vector<std::vector<char>>>& choice,
    unsigned short int a,
    unsigned short int b,
    unsigned short int p,
    const std::vector<std::tuple<unsigned short int, unsigned short int, unsigned short int>>& rooms
)
{
    for (int i = p - 1; i >= 0; --i) {
        unsigned short int n = std::get<0>(rooms[i]); // вместимость палаты
        unsigned short int a_in = std::get<1>(rooms[i]); // уже размещённых A
        unsigned short int b_in = std::get<2>(rooms[i]); // уже размещённых B

        for (unsigned short int a_out = 0; a_out <= a; ++a_out) {
            for (unsigned short int b_out = 0; b_out <= b; ++b_out) {
                // Если палата имеет нулевую вместимость, пропускаем её
                if (n == 0) { // палата 0 0 0
                    dp_curr[a_out][b_out] = dp_next[a_out][b_out];
                    choice[i][a_out][b_out] = 'N';
                    continue;
                }

                if (a_in > 0) { // уже кто-то есть (А)
                    unsigned short int can_place = (a_out < n - a_in ? a_out : n - a_in);
                    dp_curr[a_out][b_out] = dp_next[a_out - can_place][b_out] + can_place;
                    choice[i][a_out][b_out] = 'A';
                } else if (b_in > 0) { // уже кто-то есть (B)
                    unsigned short int can_place = (b_out < n - b_in ? b_out : n - b_in);
                    dp_curr[a_out][b_out] = dp_next[a_out][b_out - can_place] + can_place;
                    choice[i][a_out][b_out] = 'B';
                } else { // палата пустая
                    unsigned short int place_a = 
                        (a_out >= n)
                        ? dp_next[a_out - n][b_out] + n
                        : dp_next[0][b_out] + a_out;
                    unsigned short int place_b = 
                        (b_out >= n)
                        ? dp_next[a_out][b_out - n] + n
                        : dp_next[a_out][0] + b_out;
                    if (place_a >= place_b) {
                        dp_curr[a_out][b_out] = place_a;
                        choice[i][a_out][b_out] = 'A';
                    } else {
                        dp_curr[a_out][b_out] = place_b;
                        choice[i][a_out][b_out] = 'B';
                    }
                }
            }
        }
        dp_next = dp_curr; // далее мы будем смотреть i-1 и i (curr и next)
    } 
    return dp_curr[a][b];
}

std::vector<unsigned short int> getAPath(
    const std::vector<std::vector<unsigned short int>>& dp_curr,
    const std::vector<std::vector<unsigned short int>>& dp_next,
    const std::vector<std::vector<std::vector<char>>>& choice,
    unsigned short int a,
    unsigned short int b,
    unsigned short int p,
    const std::vector<std::tuple<unsigned short int, unsigned short int, unsigned short int>>& rooms
)
{
    std::vector<unsigned short int> path; 
    path.reserve(p);
    unsigned short int a_out = a, b_out = b;
    for (int i = 0; i < p; ++i) {
        unsigned short int n = std::get<0>(rooms[i]);
        unsigned short int a_in = std::get<1>(rooms[i]);
        unsigned short int b_in = std::get<2>(rooms[i]);
        // 0 0 0
        if(n == 0) continue;
            
        char decision = choice[i][a_out][b_out];
        if (a_in > 0) {
            unsigned short int can_place = (a_out < n - a_in ? a_out : n - a_in);
            if (can_place > 0) {
                a_out -= can_place;
                path.push_back(i + 1);
            }
        } else if (b_in > 0) {
            unsigned short int can_place = (b_out < n - b_in ? b_out : n - b_in);
            b_out -= can_place;
        } else {
            if (decision == 'A') {
                path.push_back(i + 1);
                if (a_out >= n)
                    a_out -= n;
                else
                    a_out = 0;
            } else if (decision == 'B') {
                if (b_out >= n)
                    b_out -= n;
                else
                    b_out = 0;
            }
        }
    }
    std::sort(path.begin(), path.end());
    return path;
}
