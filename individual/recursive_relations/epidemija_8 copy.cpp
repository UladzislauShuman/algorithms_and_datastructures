#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <tuple>

using namespace std;

int main() {

    std::ifstream in("input.txt");
    int a, b, p;
    in >> a >> b >> p;

    vector<tuple<int, int, int>> rooms(p);
    for (int i = 0; i < p; ++i) {
        in >> get<0>(rooms[i]) >> get<1>(rooms[i]) >> get<2>(rooms[i]);
    }

    // dp[i][a_left][b_left] - максимальное количество размещенных больных, начиная с палаты i
    // при условии, что осталось a_left больных A и b_left больных B
    vector<vector<vector<int>>> dp(p + 1, vector<vector<int>>(a + 1, vector<int>(b + 1, -1)));


    // Базовый случай: все больные размещены
    for (int i = 0; i <= p; ++i) {
        dp[i][0][0] = 0;
    }

    // Базовый случай: нет палат
    for (int i = 0; i <= a; ++i) {
        for (int j = 0; j <= b; ++j) {
            dp[p][i][j] = 0;  // Или INT_MIN, если хотите учесть, что не всех можно разместить
        }
    }


    // Заполняем таблицу dp в обратном порядке (с конца)
    for (int i = p - 1; i >= 0; --i) {
        for (int a_left = 0; a_left <= a; ++a_left) {
            for (int b_left = 0; b_left <= b; ++b_left) {

                int n = get<0>(rooms[i]);
                int a_in = get<1>(rooms[i]);
                int b_in = get<2>(rooms[i]);

                dp[i][a_left][b_left] = 0; // Initialize with the case where we don't use this room

                if (a_in > 0) {
                    int can_place = min(a_left, n - a_in);
                    dp[i][a_left][b_left] = dp[i+1][a_left - can_place][b_left] + can_place;

                } else if (b_in > 0) {
                    int can_place = min(b_left, n - b_in);
                    dp[i][a_left][b_left] = dp[i+1][a_left][b_left - can_place] + can_place;
                } else {
                    // Try placing A
                    int place_a = (a_left >= n) ? dp[i+1][a_left - n][b_left] + n : dp[i+1][0][b_left] + a_left;
                    // Try placing B
                    int place_b = (b_left >= n) ? dp[i+1][a_left][b_left - n] + n : dp[i+1][a_left][0] + b_left;

                    dp[i][a_left][b_left] = max(place_a, place_b);

                }
            }
        }
    }

    cout << dp[0][a][b] << endl;


    // Восстановление пути
    vector<int> path;
    int i = 0, a_left = a, b_left = b;
    while (i < p && (a_left > 0 || b_left > 0)) {
        int n = get<0>(rooms[i]);
        int a_in = get<1>(rooms[i]);
        int b_in = get<2>(rooms[i]);

        if (a_in > 0) {
            int can_place = min(a_left, n - a_in);
            a_left -= can_place;
            //path.push_back(i + 1); // добавляем палату, даже если размещаем больных A в палате, где они уже есть
        } else if (b_in > 0) {
            int can_place = min(b_left, n - b_in);
            b_left -= can_place;
            //path.push_back(i+1); // добавляем палату, даже если размещаем больных B в палате, где они уже есть

        } else {
           //Check if placing A in this room led to the optimal solution
            int place_a = (a_left >= n) ? dp[i+1][a_left - n][b_left] + n : dp[i+1][0][b_left] + a_left;
            int place_b = (b_left >= n) ? dp[i+1][a_left][b_left - n] + n : dp[i+1][a_left][0] + b_left;

            if(dp[i][a][b] == place_a && a_left >= n && dp[i][a][b] != place_b) {
                    path.push_back(i+1);
                    a_left -= n;
            }
            else if (dp[i][a][b] == place_a && a_left < n && dp[i][a][b] != place_b) {
                    path.push_back(i+1);
                    a_left = 0;
            }
            else if (dp[i][a][b] != place_a && dp[i][a][b] == place_b && b_left >=n) {
                    b_left -=n;
            }
            else if (dp[i][a][b] != place_a && dp[i][a][b] == place_b && b_left < n) {
                    b_left = 0;
            }

           else if(dp[i][a][b] == place_a && dp[i][a][b] == place_b && a_left >=n){
                    path.push_back(i+1);
                    a_left -= n;
            }
           else if(dp[i][a][b] == place_a && dp[i][a][b] == place_b && a_left <n){
                    path.push_back(i+1);
                    a_left = 0;
            }
        }

        i++;
    }
    sort(path.begin(), path.end());
    for (size_t j = 0; j < path.size(); ++j) {
           cout << path[j] << (j == path.size() - 1 ? "" : " ");
    }
    cout << endl;


    return 0;
}
