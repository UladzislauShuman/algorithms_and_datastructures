#include <fstream>
#include <vector>
#include <iostream>
#include <string>
#include <unordered_set>

using namespace std;

/*
идея простая
берём строку
и бинарным поиском ищем наше К
итерация для К выглядит так
    считаем хеш каждой подстроки
        использовать буду обратный полиномиальный хеш (проще считать)
    и проверяем, нет ли дубликатов
*/


int find_min_substring_length(string s); 

int main() {
    std::ifstream in("input.txt");
    std::ofstream out("output.txt");

    string s; in >> s;

    cout << find_min_substring_length(s);

    in.close(); out.close();
    return 0;
}

int find_min_substring_length(string s) {
    int left = 0, right = s.size();
    int result = s.size();

    int medium; bool isUnique;
    while (left <= right) {
        medium = (left + right) / 2;
        isUnique = true;
        int k = 911382629;
        long long mod = 1000000000000000003LL;

        long long k_power = 1;
        for (int i = 0; i < medium - 1; ++i)  // вот здесь я считаю k^(medium - 1)
            k_power = k_power * k % mod;
        
        unordered_set<long long> hashes;
        long long temp_hash = 0;
        
        // сама проверка начинается тут
        for (int i = 0; i < medium; ++i) { // вычисляем перфикс 
            temp_hash = (temp_hash * k + s[i]) % mod;
        }
        hashes.insert(temp_hash);

        for (int i = medium; medium < s.size(); ++i) {
            temp_hash = (temp_hash - s[i - medium] * k_power % mod + mod) % mod; // удаляем букву левую
            temp_hash = (temp_hash * k + s[i]) % mod;

            if (hashes.count(temp_hash)) { // был ли уже до этого такой же хеш?
                isUnique = false;
                break;
            }
            hashes.insert(temp_hash);
        }
        //

        //проверяем, что произашло за эту итерацию поиска
        if (isUnique == true) { // если все уникальны 
            result = medium; // запоминаем результат
            right = medium; // и понимаем, что ответ где-то левее
        } else {
            left = medium + 1; // иначе
        }
    }

    return result;
}