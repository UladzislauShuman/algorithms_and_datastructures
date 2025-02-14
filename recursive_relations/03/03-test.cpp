#include <fstream>
#include <vector>

const int MOD_NUM = 1000000007;
void createFactorial(std::vector<long long>& factorial, int n) //  ДП назад
{
    for (int i = 1; i <= n; ++i)
        factorial[i] = (factorial[i - 1] * i) % MOD_NUM; 
}

long long getInversForMod(long long a);
std::vector<long long> createFactorialInverseFromFactorial(std::vector<long long>& factorial, int n)
{
    std::vector<long long> factorial_inverse(n+1,1);
    factorial_inverse[n] = getInversForMod(factorial[n]); 
    for (int i = n - 1; i >= 1; --i)
        factorial_inverse[i] = (factorial_inverse[i + 1] * (i + 1)) % MOD_NUM; // 1/(n * n-1 * ...) * n = 1/(n-1 * n-2 *...) 
    return factorial_inverse;
}

long long getInversForMod(long long a) {
    long long b = 1; // a * b == 1 mod MOD_NUM
    long long power = MOD_NUM - 2; // из т Эйлера 
    while (power > 0) { // чтобы посчитать a^(m-2) -- бинарное возведение в степень
        if (power & 1) { // если 1 -- нечётное, 0 -- чётное
            b = (b * a) % MOD_NUM;
        }
        a = (a * a) % MOD_NUM;
        power >>= 1;
    }
    return b;
}

int main() {
    std::ifstream in("input.txt");
    std::ofstream out("output.txt");

    
    int n,k;
    in >> n >> k;
    if (k > ((n+1)>>1)) 
    {
        k = n - k;
    }
    
    // как бы я не хотел считать факториал, но вариантов у меня пока нет
    // считаем факториал, "сохраняя историю", и для эканомии места, согласно свойством
    // модульной арифметики, считаем
    std::vector<long long> factorial(n+1,1); createFactorial(factorial, n);
    // элементы вида a*b = 1 mod MOD
    std::vector<long long> factorial_inverse; factorial_inverse = createFactorialInverseFromFactorial(factorial, n);    
    out << (((factorial[n] * factorial_inverse[k]) % MOD_NUM) * factorial_inverse[n - k]) % MOD_NUM;

    in.close(); out.close();
    return 0;
}

