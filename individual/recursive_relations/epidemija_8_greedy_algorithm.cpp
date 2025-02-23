#include <fstream>
#include <vector>
#include <iostream>
#include <algorithm>
#include <sstream>

typedef unsigned short int USI;

/*
все мои контр примеры, что я успел придумать, оно проходит
но все равно есть доля неуверенности
вашему вниманию -- жадный алгоритм
*/

struct Room
{
    bool isFillWithA = false; // заполняли ли мы вообще его больными А из машины?
    USI index; //индекс комнаты
    USI n; // вместимость комнаты
    USI a_in; // сколько там больных А
    USI b_in; // Б
    Room() : index(0), n(0), a_in(0), b_in(0) {}
    Room(USI index, USI n, USI a_in, USI b_in) : index(index), n(n), a_in(a_in), b_in(b_in) {}
    USI getCapacity(){return n - a_in - b_in;}; // получить текущую вместимость (по ней и сравниваются комнаты)
    // сравнивая по убыванию по этому методу, мы получим что -- заполненные вниз, а пустые и с большой вместимостью выше
    void fillRoom(USI& a, USI& b) {
        if(this->a_in > 0 and this->a_in != this->n) // второе условие, чтобы не заполнять уже заполненный (50 50 0 к примеру)
            this->fullRoomA(a);
        else if(this->b_in > 0 and this->b_in != this->n) 
            this->fullRoomB(b);
    }

    void fullRoomA(USI& a){
        if (a > 0) // чтобы не заполнять "никем" (т.е. -- когда А уже изначально равно 0, к примеру)
        {
            a = a > (this->n - this->a_in)?  a - (this->n - this->a_in) : 0;
            this ->a_in = this->n;
            this->isFillWithA = true;   
        }
    }

    void fullRoomB(USI& b){
        if (b > 0)
        {
            b = b > (this->n - this->b_in)?  b - (this->n - this->b_in) : 0;
            this ->b_in = this->n; 
        }
    }

    bool isFull(){return this->a_in == this->n or this->b_in == this->n;}
};


int main() {
    std::ifstream in("input.txt");
    std::ofstream out("output.txt");

    USI a,b,p;
    in >> a >> b >> p;
    USI maxSum = a + b; // тк я передаю по ссылке a и b, то мне нужно не забыть о "а сколько максимум я мог вообще заполнить?"

    std::vector<Room> rooms(p);
    for (int i = 0; i < p; ++i)
    {
        rooms[i].index = i + 1;
        in >> rooms[i].n >> rooms[i].a_in >> rooms[i].b_in;
        rooms[i].fillRoom(a,b);
    }

    std::sort(rooms.begin(), rooms.end(), [](Room& a, Room& b) {
        return a.getCapacity() > b.getCapacity();
    });

    int i = 0;
    while ((a > 0 || b > 0) and i < p and !rooms[i].isFull()) { 
        /*
        у нас сейчас так отсортированно, что пустые сверху, а полные снизу
        если мы уже добрались до полной, то дальше проверять нет смысла
        */
        if (a >= b)
            rooms[i].fullRoomA(a);
        else // a < b
            rooms[i].fullRoomB(b);
        i++;
    }

    out << maxSum - a - b << "\n";
    
    if (a+b == 0) {
        std::sort(rooms.begin(), rooms.end(), [](const Room& a, const Room& b) {
            return a.index < b.index;
        }); //возрващаем исходную индексацию

        std::ostringstream ss;
        for (const Room& room : rooms) 
            if (room.isFillWithA)
                ss << room.index <<" ";
        
        std::string result = ss.str();
        if (!result.empty()) 
            result.pop_back(); 
       out << result;
    }
    
    in.close(); out.close();
    return 0;
}