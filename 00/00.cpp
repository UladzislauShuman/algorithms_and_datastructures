#include <iostream>
#include <set>
#include <fstream>

int main()
{
    std::set<int> numbers;
    
    std::ifstream inStream("input.txt");
    std::ofstream outStream("output.txt");
    
    
    
    int temp_number;
    while (inStream >> temp_number)
        numbers.insert(temp_number);
        
    
    long long result = 0;
    for (int number : numbers)
        result += number;
    
    outStream << result;
    
    inStream.close();
    outStream.close();

    return 0;
}