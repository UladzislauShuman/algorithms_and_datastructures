#include <fstream>
#include <vector>
#include <iostream>
#include <set>
#include <algorithm>
#include <limits>
#include <iterator>

typedef unsigned short int USI;

struct Room
{
    static std::vector<USI> fullA;
    USI index;
    USI n;
    USI a_in;
    USI b_in;
    Room() : index(0), n(0), a_in(0), b_in(0) {}
    Room(USI index, USI n, USI a_in, USI b_in) : index(index), n(n), a_in(a_in), b_in(b_in) {}
    USI getCapacity() const {return n - a_in - b_in;}
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
            Room::fullA.push_back(this->index);   
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

struct DP_element
{
    // <сумма, которую мы можем достичь, DP_element>
    static std::set<DP_element> dp;
    static DP_element not_found_element(0, std::numeric_limits<int>::min(), 0);
    static DP_element& UpperBound(int A);
    static DP_element& EqualBound(int A);
    static DP_element& LowerBound(int A);
    
    USI last_room_index = 0; // * сумма всех предыдущих * + ПОСЛЕДНЯЯ(эта) добавленная комната
    int current_sum = 0;
    int sum_before = 0; // то, что было до неё, чтобы потом далее в карте перейти к ней
    DP_element(USI last_room_index,int current_sum, int sum_before) :
        last_room_index(last_room_index), 
        current_sum(current_sum),
        sum_before(sum_before)
        {}
    DP_element(const Room& room, const DP_element& element) {
        this->last_room_index = room.index;
        this->current_sum= element.current_sum + room.getCapacity();
        this->sum_before = element.current_sum;
    }

    bool operator< (const DP_element& other) const {
        return this->current_sum < other.current_sum;
    }
    
    bool operator== (const DP_element& other) const {
        return this->current_sum == other.current_sum && this->sum_before == other.sum_before;
    }
};
std::ostream& operator<< (std::ostream& out, const DP_element& element) {
    out     << "DP_element last_room_index: " << element.last_room_index 
                           << " sum_before: " << element.sum_before 
                           << " current_sum: " << element.current_sum 
                           << "\n";
    return out;
}
std::vector<USI> Room::fullA;
std::set<DP_element> DP_element::dp;

DP_element& DP_element::UpperBound(int A)
{
    DP_element dummy(0, A, 0); // Создаем фиктивный элемент для поиска
    auto it = dp.upper_bound(dummy);

    if (it == dp.end()) {
        // Если больше A нет элементов, возвращаем последний элемент (или создаем новый с максимальным значением, в зависимости от логики)
        static DP_element max_element(0, std::numeric_limits<int>::max(), 0); // Статический объект для возврата
        return max_element; 
    } else {
        return const_cast<DP_element&>(*it); //  Возвращаем ссылку на найденный элемент. const_cast необходим, т.к. set хранит const элементы
    }
}

DP_element& DP_element::LowerBound(int A)
{
    DP_element dummy(0, A, 0); // Создаем фиктивный элемент для поиска

    auto it = dp.upper_bound(dummy); // Находим первый элемент > A

    if (it == dp.begin()) {
        // Если все элементы больше A, возвращаем первый элемент (или создаем минимальный, если нужно)
        static DP_element min_element(0, std::numeric_limits<int>::min(), 0);
        return min_element; // Возвращаем первый элемент
    } else {
        // Иначе, возвращаем элемент, предшествующий найденному (т.е. <= A)
        it--;
        return const_cast<DP_element&>(*it); //  Возвращаем ссылку на найденный элемент
    }
}

DP_element& DP_element::EqualBound(int A)
{
    DP_element dummy(0, A, 0); // Создаем фиктивный элемент для поиска
    auto it = dp.find(dummy);

    if (it != dp.end()) {
        return const_cast<DP_element&>(*it); // Элемент найден
    } else {
        return DP_element::not_found_element;
    }
}



int main() {
    std::ifstream in("input8.txt");
    std::ofstream out("output.txt");

    USI a,b,p;
    in >> a >> b >> p;
    Room::fullA.reserve(p);
    int p_size_sum = 0;

    std::vector<Room> rooms(p);
    for (int i = 0; i < p; ++i)
    {
        rooms[i].index = i + 1;
        in >> rooms[i].n >> rooms[i].a_in >> rooms[i].b_in;
        rooms[i].fillRoom(a,b);
        p_size_sum += rooms[i].getCapacity();
    }

    // log
    std::cout << p_size_sum << std::endl;
    

    // заполняем наш dp-массив
    std::vector<DP_element> temp; temp.reserve(p);
    DP_element::dp.insert(DP_element(0, 0, 0)); // вставляем в начало сумму 0
    for(const auto room : rooms) { // std::vector<Room> rooms(p);
        if (room.getCapacity() == 0) continue;
        for (const auto current_DP_element : DP_element::dp) { // std::set<DP_element> DP_element::dp;
            temp.push_back(DP_element(room, current_DP_element)); // при итерации по set добавлять в него нельзя (точнее можно, просто всё поломаеться)
        }
        DP_element::dp.insert(temp.begin(), temp.end());
        temp.clear();
    }
    // log
    for (const auto& element : DP_element::dp) {
        std::cout << element;
    }


    //ТЕСТ
    // находим UpperBound элемент И LowerBound элемент dp по A 
    DP_element upper_ = DP_element::UpperBound(a);
    DP_element equal_  = DP_element::EqualBound(a);
    DP_element lower_ = DP_element::LowerBound(a);
    // log
    std::cout   << "\n" 
                << "LowerBound(A): " << lower_
                << "EqualBound(A): " << equal_ 
                << "UpperBound(A): " << upper_
                << "\n";

    /*
    если мы нашли, A задаётся комнатами
    то есть ли смысл искать LowerBound?
        я думаю нет, тк -- Эта комната передастся В
        и будет либо
            В заполнет те же места, что и А (просто будут гулять А, а не В)
            либо мы и В всё так и не заполнили -- ну и будут гулять и А, и В
        т.е. -- смысла нет
        с UpperBound может быть "хуже"
    и EqualBound -- будет самым оптимальным решением 
    если же все же равного нет, то -- искать будем И Lower и Upper
    и некоторый из них будет оптимальным

    если Lower -- то гуляют некоторые А
    если Upper -- то Могут гулять некоторые B
    Lower:
        lowerSum = Lower.current_sum + B <= (total - Lower.current_sum) ? B : (total - Lower.current_sum);
    Upper:
        uppeerSum = A + B <= (total - Upper.current_sum) ? B : (total - Upper.current_sum);

    DP_element result = lowerSum > upperSum? Lower : Upper

        
    */
   DP_element optimal;
   DP_element equal  = DP_element::EqualBound(a);
   if (equal != DP_element::not_found_element) { // если всё же в dp есть место для A
        optimal = equal; 
        /*
        получается A все сидят
        и проверяем В
        */
        if (b - optimal.current_sum > 0) { // получается, что не все b сидят, все палаты заняти и мы выводим только max
            out << p_size_sum << "\n";
        } else { // b - optimal.current_sum <= 0 -- т.е. A и B сидят, зн надо выводить путь А
            //...
        }
   }
   else { // иначе ищем Lower и Upper
        DP_element upper = DP_element::UpperBound(a);
        DP_element lower = DP_element::LowerBound(a);

        
   
        if (
            lower.current_sum + (b <= (p_size_sum - lower.current_sum) ? b : (p_size_sum - lower.current_sum)) // lower
            >
            a + (b <= (p_size_sum - upper.current_sum) ? b : (p_size_sum - upper.current_sum)) // upper
        ) { 
            /*
            т.е. -- когда мы не все А заполняем, то помещаем тем самым больше В -- отсюда может и быть более Оптимальным
            но да -- тем самым мы не всех помещаем
            зн только максимум вывести
            */
           out << p_size_sum << "\n";
        }
        else { 
            /*
            по крайне мере А все сидят
            надо проверить В
            */
           if (b <= (p_size_sum - upper.current_sum)) // если и Б поместили, то надо вывести путь для А
           {

           }
        }
    }



    //DP_element lastElement = std::orev(DP_element::dp.end());
    

    
    in.close(); out.close();
    return 0;
}