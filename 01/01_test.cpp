#include <iostream>
#include <vector>
#include <fstream>


//тк в прошлых попытках я искал Линейно l и r, то наверное были проблемы со времнем тк тест мог
//состоять из просто тучи одинаковых x, из-за чего линейный алгоритм и показывал себя плохо
int l(std::vector<int>& array, int& x, int& left, int& middle)
{
    int left_ = left;
    int right_ = middle;

    while(left_ < right_)
    {
        int mid = (left_ + right_)/2;
        if (x <= array[mid])
            right_ = mid;
        else
            left_ = mid + 1;
    }
    if (left_ < array.size()) return left_;
    else return array.size();
}


int r(std::vector<int>& array, int& x,int& middle, int& right)
{
    int left_ = middle;
    int right_ = right;

    while(left_ < right_)
    {
        int mid = (left_ + right_)/2;
        if (x < array[mid])
            right_ = mid;
        else
            left_ = mid + 1;
    }
    if (left_ < array.size()) return left_;
    else return array.size();
}

int main()
{
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::ifstream inputFile("input.txt");
    std::ofstream outputFile("output.txt");

    int n; 
    inputFile >> n;
    std::vector<int> array(n);
    
    for (int i = 0;i < n; ++i)
    inputFile >> array[i];
    
    int k; inputFile >> k;
    
    int x;
    bool b; // есть ли X в массиве
    

    for (int i = 0; i < k; ++i)
    {
        inputFile >> x;
        
        //сам бинарный поиск
        int left = 0; int right = n - 1; int mid = -1;
        while(true) 
        {
            if (left > right) 
            {
                b = false;
                break;
            }
            mid = (left + right)/2;
            if (array[mid] == x) // т.е. -- мы нашли
            {
                b = true;
                break;
            }
             //иначе, продолжаем поиск, пока не Найдём, Либо покажем вариант отсуствия
            // урезаем на половину рассматриваемую область
            if (array[mid] < x) left = mid + 1;
            else right = mid - 1;
        }

        //забыл учесть случай, когда n = 0 и k = 0
        if(mid != -1){
            outputFile 
                    << b << " " 
                    << l(array,x,left,mid) << " " // index первого, что array[l] >= x
                    << r(array,x,mid,n) << "\n"; // index первого, что array[r] > x
        }
        else{
            outputFile << "0 0 0\n";
        }
    }
    return 0;
}