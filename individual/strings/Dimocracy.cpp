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

    cout << find_min_substring_length(s) << endl;
    cout << find_min_substring_length("ABCDEF") << endl;
    cout << find_min_substring_length("AAAAAA") << endl;
    cout << find_min_substring_length("ABCABCABC") << endl;
    cout << find_min_substring_length("ABCDEFGABCDEFGABCDEFGABCDEFG") << endl;
    //cout << find_min_substring_length(string(100000,'A') + string(100000,'B')) << endl;

    in.close(); out.close();
    return 0;
}

int find_min_substring_length(string s) {
    cout << s << endl;
    int left = 1;       
    int right = s.size();       
    int result = s.size();

    // для уменьшения коолизии
    const unsigned long long k1 = 37, mod1 = 1000000007;
    const unsigned long long k2 = 257, mod2 = 1000000009; // я решил взял простое число поменьше, чтобы было проще считать
    //чтобы лишний раз не считать
    vector<unsigned long long> k1_pows(s.size()+1, 1);
    vector<unsigned long long> k2_pows(s.size()+1, 1);
    
    for (int i = 1; i <= s.size(); ++i) {
        k1_pows[i] = (k1_pows[i-1] * k1) % mod1;
        k2_pows[i] = (k2_pows[i-1] * k2) % mod2;
    }

    int medium; bool isUnique;
    unordered_set<unsigned long long> hashes1, hashes2;
    hashes1.reserve(s.size() - medium + 1);
    hashes2.reserve(s.size() - medium + 1);
    while (left <= right) {
        medium = left + (right - left) / 2;
        isUnique = true;
        hashes1.clear();
        hashes2.clear();

        unsigned long long temp_hash1 = 0, temp_hash2 = 0;
        
        // сама проверка начинается тут
        for (int i = 0; i < medium; ++i) { // вычисляем перфикс 
            temp_hash1 = (temp_hash1 * k1 + s[i]) % mod1;
            temp_hash2 = (temp_hash2 * k2 + s[i]) % mod2;
        }
        hashes1.insert(temp_hash1);
        hashes2.insert(temp_hash2);

        for (int i = medium; i < s.size(); ++i) { 
            temp_hash1 = (temp_hash1 - s[i - medium] * k1_pows[medium-1] % mod1 + mod1) % mod1;
            temp_hash1 = (temp_hash1 * k1 + s[i]) % mod1;

            temp_hash2 = (temp_hash2 - s[i - medium] * k2_pows[medium-1] % mod2 + mod2) % mod2;
            temp_hash2 = (temp_hash2 * k2 + s[i]) % mod2;


            if (hashes1.count(temp_hash1) && hashes2.count(temp_hash2)) { // был ли уже до этого такой же хеш?
                isUnique = false;
                break;
            }
            hashes1.insert(temp_hash1);
            hashes2.insert(temp_hash2);
        }
        //

        //проверяем, что произашло за эту итерацию поиска
        if (isUnique) { // если все уникальны 
            result = medium; // запоминаем результат
            right = medium - 1; // сужаем правую границу (теперь right исключается)
        } else {
            left = medium + 1; // сужаем левую границу
        }
    }

    return result;
}