#include <fstream>
#include <vector>
#include <string>
#include <iostream>

/*
14 = 2^3 + 2^2 + 2^1
FindSum(0,14) = t[2^3] + t[2^3 + 2^2] + t[2^3 + 2^2 + 2^1];
5 = 2^2 + 2^0;
FindSum(0,5) = t[2^2] + t[2^2 + 2^0];


вопрос -- как в процессе вычислений мы умудряемся хранить [1-4] + [5]?
предположение : вероятно это связано с постепеленным добавлением элементов
init : массив из нулей
алгоритм Add
i = position
i += i & (-i)
i < количество коробок

как вычисляется Это
i += i & (-i)
пусть i = 5
5  =            0101
-5 = 1010 + 1 = 1011
                ----
                0001 = 1

5 + 2^0
6  = 0110
-6 = 1010
&     0010 = 2

5 + 2^0 + 2^1
8  = 01000
-8 = 11000
&    01000 = 4

...


GetSum -- уже работает на построном -- нашем идеально работающем представлении

14  = 01110
-14 = 10010
&     00010 = 2

14 - 2^1
12  = 01100
-12 = 10100
&     00100 = 4

14 - 2^1 - 2^2 
8  = 01000
-8 = 11000
&    01000 = 8

14 - 2^1 - 2^2 - 2^3
0  = 00
-0 = 10
&    00

*/
struct Command {
    bool type; // true - FindSum, false - Add
    int param1;
    int param2;
    Command(std::string type_, int param1, int param2) :  param1(param1), param2(param2) {
        if (type_ == "FindSum") {
            this->type = true;
        }
        else { // Add
            this->type = false;
        }
    }
};
class FenvikTree {
    private:
        std::vector<long long> tree;

        long long GetSum(int position) {
            long long sum = 0;
            for (int i  = position+1; i > 0; i -= i & (-i) ) {
                sum += tree[i-1];
            }
            return sum;
        }
    public:
        FenvikTree(int size) {
            tree = std::vector<long long>(size,0);
        }
        long long FindSum(int left, int right) { //[left,right)
            return GetSum(right - 1) - GetSum(left-1);
        }
        void Add(int position, long long addition) {
            for (int i = position + 1; i <= this->tree.size(); i += i & (-i)) {
                tree[i-1] += addition;
            }
        }

        void show() {
            for (const auto& element : this->tree) 
                std::cout << element << " ";
            std::cout << std::endl;
            for (int i = 0; i < this->tree.size(); ++i) {
                std::cout << this->GetSum(i) << " ";
            }
            std::cout << std::endl;
            
        }
};

int main() {

    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    
    //считываем данные
    int n; std::cin >> n;

    int temp;
    FenvikTree tree(n);
    for(int i = 0; i < n; ++i) {
        std::cin >> temp;
        tree.Add(i,temp);
    }

    // считываем команды
    int q; std::cin >> q;
    std::vector<Command> commands; commands.reserve(q);
    std::string command; int param1, param2;
    for (int i = 0; i < q; ++i) {
        std::cin >> command >> param1 >> param2;
        commands.push_back(Command(command, param1, param2));
    }
    
    
    for (const auto& command_ : commands) {
        if (command_.type)
            std::cout << tree.FindSum(command_.param1, command_.param2) << "\n";
        else
            tree.Add(command_.param1, command_.param2);
    }

    return 0;
}