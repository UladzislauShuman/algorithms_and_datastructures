#include <fstream>
#include <vector>
#include <climits>

int getUpperBound(const std::vector<int>& sequence, const int& n,const int& x) ;
int getLISLength(const std::vector<int>& sequence,const int& n);

int main() {
    std::ifstream in("input.txt");
    std::ofstream out("output.txt");

    int n; in >> n;
    std::vector<int> sequence(n);
    for (int i = 0; i < n; ++i) 
        in >> sequence[i];
    out << getLISLength(sequence, n);
    
    in.close(); out.close();
    return 0;
}

int getLISLength(const std::vector<int>& sequence,const int& n) {
    std::vector<int> dp(n, INT_MAX); // иначе он случайными числами заполниться
    int lisLength = 0;
    int i;
    /*
    предположим, что есть 1 2 4 7 12 и на вход 6
    */
    for (int j = 0; j < n; ++j) {
        i = getUpperBound(dp, lisLength, sequence[j]);
        if (i < lisLength)
            dp[i] = sequence[j]; //  getUpperBound(6) = 7, и меняем 1 2 4 6 12
            // т.о. есть две подпосл 1 2 4 6 (предположим, что за время ничего не менялось И это чистая подпосл)
            // и 1 2 4 ** 12
            // теперь, если добавит 10, то getUpperBound(10) = length , и теперь у нас меняется наша
            // "бывшая"  перспективная подпосл : 1 2 4 6 10
        else {
            dp[lisLength] = sequence[j]; 
            ++lisLength;
        }
    }
    return lisLength; 
}

int getUpperBound(const std::vector<int>& dp, const int& lisLength,const int& x) {
    int left = 0, right = lisLength, mid; // [l, r)
    while (left < right) {
        mid = (left + right) >> 1;
        if (x <= dp[mid]) // почему всё же <= --
        // ситуация -- 1 2 4 6 12 и вход 6
        // если <, то 1 2 4 6 6 -- дубликат => !строгая
        // <=, то дубликаты не появятся => останется строгой
            right = mid;
        else // x > dp[mid]
            left = mid + 1;
    }
    return left;
}