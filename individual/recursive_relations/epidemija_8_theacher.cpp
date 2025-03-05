#include <fstream>
#include <vector>
#include <iostream>
#include <set>
#include <algorithm>
#include <limits>
#include <iterator>
#include <vector>
#include <string>
#include <sstream>

typedef unsigned short int USI;

struct Room
{
    static int p_filled; // когда мы счиытваем и заполняем не пустые палаты, то инфа о таких лежачишь храниться тут
    static int p_sum_empty; // полная сумма объймом палат

    static std::vector<USI> fullA; // индексы тех, что хранят в себе приезжах
    USI index;
    USI n;
    USI a_in;
    USI b_in;
    Room() : index(0), n(0), a_in(0), b_in(0) {}
    Room(USI index, USI n, USI a_in, USI b_in) : index(index), n(n), a_in(a_in), b_in(b_in) {}
    USI getCapacity() const {return n - a_in - b_in;}
    bool isFull(){return this->a_in == this->n or this->b_in == this->n;}

    void fillRoom(USI& a, USI& b) {
        if(this->a_in > 0 and this->a_in != this->n) // второе условие, чтобы не заполнять уже заполненный (50 50 0 к примеру)
            this->fillRoomA(a);
        else if(this->b_in > 0 and this->b_in != this->n) 
            this->fillRoomB(b);
    }
    void fillRoomA(USI& a){
        if (a > 0) // чтобы не заполнять "никем" (т.е. -- когда А уже изначально равно 0, к примеру)
        {
            Room::p_filled += a < (this->n - this->a_in) ? a : (this->n - this->a_in); 
            a = a > (this->n - this->a_in)?  a - (this->n - this->a_in) : 0;
            this ->a_in = this->n;
            Room::fullA.push_back(this->index);  // не забываем, что  они у нас тут лежат 
            
        }
    }
    void fillRoomB(USI& b){
        if (b > 0)
        {
            Room::p_filled += b < (this->n - this->b_in) ? b : (this->n - this->b_in);
            b = b > (this->n - this->b_in)?  b - (this->n - this->b_in) : 0;
            this ->b_in = this->n; 
        }
    }
};

/*
в этой структуре и храниться наш std::set с "достигаемыми значениеями"
*/
struct DP_element
{
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
    DP_element() : last_room_index(0), current_sum(0), sum_before(0){}

    /*
    <сумма, которую мы можем достичь, DP_element> 
    так же -- тепеь нам не нужно знать факт "достигаемости"
    */
    static std::set<DP_element> dp;
    static DP_element not_found_element; // своего рода nullptr
    static DP_element UpperBound(int A); // ближайший сверху от А, что мы можем достичь
    static DP_element EqualBound(int A);
    static DP_element LowerBound(int A); // аналогично снизу
    
    USI last_room_index = 0; 
    int current_sum = 0;// * сумма всех предыдущих * + ПОСЛЕДНЯЯ(эта) добавленная комната
    int sum_before = 0; // то, что было до неё, чтобы потом далее в карте перейти к ней
    
    // нужны, чтобы хранить структуру в set
    bool operator< (const DP_element& other) const {return this->current_sum < other.current_sum;}
    bool operator== (const DP_element& other) const {return this->current_sum == other.current_sum;}
    bool operator!= (const DP_element& other) const {return this->current_sum!= other.current_sum;}
};

std::ostream& operator<< (std::ostream& out, const DP_element& element) {  
    out     << "DP_element last_room_index: " << element.last_room_index 
                           << " sum_before: " << element.sum_before 
                           << " current_sum: " << element.current_sum 
                           << "\n";
    return out;
}

int Room::p_filled = 0;
int Room::p_sum_empty = 0;
std::vector<USI> Room::fullA;
std::set<DP_element> DP_element::dp;
DP_element DP_element::not_found_element = DP_element(0, -1, 0);

DP_element DP_element::UpperBound(int A)
{
    DP_element dummy(0, A, 0); // Создаем фиктивный элемент для поиска
    auto it = dp.upper_bound(dummy); // std::set::upper_bound

    if (it == dp.end()) {
        // Если больше A нет элементов, возвращаем последний элемент (или создаем новый с максимальным значением, в зависимости от логики)
        static DP_element max_element(0, std::numeric_limits<int>::max(), 0); // Статический объект для возврата
        return max_element; 
    } else {
        return const_cast<DP_element&>(*it); //  Возвращаем ссылку на найденный элемент. const_cast необходим, т.к. set хранит const элементы
    }
}
DP_element DP_element::LowerBound(int A)
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
DP_element DP_element::EqualBound(int A)
{
    DP_element dummy(0, A, 0); // Создаем фиктивный элемент для поиска
    auto it = dp.find(dummy);

    if (it != dp.end()) {
        return const_cast<DP_element&>(*it); // Элемент найден
    } else {
        return DP_element::not_found_element;
    }
}
std::string getAPath(DP_element& element);

int main() {
    std::ifstream in("input.txt");
    std::ofstream out("output.txt");

    // заполняем данные
    USI a,b,p;
    in >> a >> b >> p;
    Room::fullA.reserve(p);
    std::vector<Room> rooms(p);
    for (int i = 0; i < p; ++i)
    {
        rooms[i].index = i + 1;
        in >> rooms[i].n >> rooms[i].a_in >> rooms[i].b_in;
        rooms[i].fillRoom(a,b);
        Room::p_sum_empty += rooms[i].getCapacity();
    }

    // заполняем наш dp-массив
    /*
    // при итерации по set добавлять в него нельзя (точнее можно, просто всё поломаеться)
    поэтому -- мы сначала создаём временный, туда добавляем
    потом переносим в наш И так по новой
    */
    std::vector<DP_element> temp; temp.reserve(p);
    DP_element::dp.insert(DP_element(0, 0,-1)); // вставляем в начало сумму 0
    for(const auto room : rooms) {
        if (room.getCapacity() == 0) continue; // если уже заполнено, то не надо оно
        for (const auto current_DP_element : DP_element::dp) {  // итериируемся по set
            temp.push_back(DP_element(room, current_DP_element)); 
        }
        DP_element::dp.insert(temp.begin(), temp.end());
        temp.clear();
    }

    // в этом блоке я вывожу ответ
    // сначала -- сколько въехало
    // и если все, то вывести путь А
    DP_element optimal;
    DP_element equal  = DP_element::EqualBound(a);
    if (equal != DP_element::not_found_element) { // если всё же в dp есть место для A
        optimal = equal; 
        /*
        получается A все сидят
        и проверяем В
        */
        
        if (b - (Room::p_sum_empty - optimal.current_sum) > 0) { // получается, что не все b сидят, все палаты заняти и мы выводим только max
            out << Room::p_sum_empty + Room::p_filled << "\n";
        } else { // b - optimal.current_sum <= 0 -- т.е. A и B сидят, зн надо выводить путь А
            out << a + b + Room::p_filled << "\n";
            out << getAPath(optimal);
        }
    }
    else { // иначе ищем Lower и Upper
        DP_element upper = DP_element::UpperBound(a);
        DP_element lower = DP_element::LowerBound(a);
        if (
            lower.current_sum + (b <= (Room::p_sum_empty - lower.current_sum) ? b : (Room::p_sum_empty - lower.current_sum)) // lower
            >
            a + (b <= (Room::p_sum_empty - upper.current_sum) ? b : (Room::p_sum_empty - upper.current_sum)) // upper
        ) { 
            if (b - (Room::p_sum_empty - lower.current_sum) > 0) { // получается, что не все b сидят, все палаты заняти и мы выводим только max
                out << Room::p_sum_empty + Room::p_filled<< "\n";
            } else { // b - optimal.current_sum <= 0 -- т.е. A и B сидят, зн надо выводить путь А
                out << lower.current_sum + b << "\n";
            }
        }
        else { 
            if (b - (Room::p_sum_empty - upper.current_sum) > 0) { // получается, что не все b сидят, все палаты заняти и мы выводим только max
                out << a + (Room::p_sum_empty - upper.current_sum) + Room::p_filled << "\n";
            } else { // b - optimal.current_sum <= 0 -- т.е. A и B сидят, зн надо выводить путь А
                out << a + b + Room::p_filled<< "\n";
                out << getAPath(upper);
            }
        }
    }
    in.close(); out.close();
    return 0;
}


std::string getAPath(DP_element& element)
{
    // читаем справа налево
    std::vector<USI> path = Room::fullA;
    path.push_back(element.last_room_index);
    for (auto it = DP_element::dp.find(DP_element(0 ,element.sum_before, 0)); it != DP_element::dp.end();) {
        path.push_back(it->last_room_index);
        it =  DP_element::dp.find(DP_element(0 ,it->sum_before, 0));
    }

    // теперь нужно не убвыание получить, а возрастание И строку
    std::sort(path.begin(), path.end());
    std::ostringstream out_result;
    for (auto it = ++path.begin(); it != path.end(); ++it) {
        out_result << *it << " ";
    }
    std::string result = out_result.str();
    if (!result.empty()) // чтобы удалить пробел в конце
        result.pop_back();

    return result;
}