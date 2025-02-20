#include <fstream>
#include <vector>

int fun(int a, int b, std::vector<std::vector<int>>& hospital_ward, int index);

int main() {
    std::ifstream in("input.txt");
    std::ofstream out("output.txt");

    int a; in >> a;
    int b; in >> b;
    int p; in >> p;

    std::vector<std::vector<int>> hospital_ward(p, std::vector<int>(3,0));
    for (int i = 0; i < p; ++i) 
    {
        in >> hospital_ward[i][0];
        in >> hospital_ward[i][1];
        in >> hospital_ward[i][2];
    }

    fun(a,b, hospital_ward, 0);
    
    


    in.close(); out.close();
    return 0;
}

int fun(int a, int b, std::vector<std::vector<int>>& hospital_ward, int index)
{
    if (hospital_ward[index][1] != 0) 
    {
        a -= (hospital_ward[index][0] - hospital_ward[index][1]);
        index++;
    }
    else if (hospital_ward[index][2] != 0) // p_a == 0 and p_b != 0
    {
        b -= (hospital_ward[index][0] - hospital_ward[index][2]);
        index++;
    }
    else { // p_a == 0 and p_b == 0
        int L = fun(a - hospital_ward[index][0], b, hospital_ward, index + 1);
        int R = fun(a, b - hospital_ward[index][0], hospital_ward, index + 1);
        if (
            L > R
        )
        {
            
        }
    }
    return 1;
}