#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>

using namespace std;

/*
    исходя из прочитанного в интернете
    я решил отсортировать по p на убывание, потом по s (ну и по id)
    сохранить чередование загрузки на процессор
        но для случая, когда одна длинная задача по вычислениям, из-за чего на процессор другой 
        грузиться две задачи подряд, по сути
        все хорошо в этом плане
*/

struct Task {
    int id;
    int cpu_num; 
    long long p;
    long long s; 

    // сортируем по убыванию p,
    // если равны — по убыванию s,
    // при равенстве — по возрастанию id (не сказать, что это обятально -- просто для стабильности)
    bool operator<(const Task& other) const {
        if (p != other.p) {
            return p > other.p;
        }
        if (s != other.s) {
            return s > other.s;
        }
        return id < other.id;
    }
};

int main() {
    ifstream in("input.txt");
    ofstream out("output.txt");

    // считываем
    int n1, n2;
    in >> n1 >> n2;
    
    vector<Task> tasks1_sorted(n1);
    for (int i = 0; i < n1; ++i) {
        tasks1_sorted[i].id = i + 1; // нумерация с 1
        tasks1_sorted[i].cpu_num = 1;
        in >> tasks1_sorted[i].p >> tasks1_sorted[i].s;
    }

    vector<Task> tasks2_sorted(n2);
    for (int i = 0; i < n2; ++i) {
        tasks2_sorted[i].id = i + 1;
        tasks2_sorted[i].cpu_num = 2;
        in >> tasks2_sorted[i].p >> tasks2_sorted[i].s;
    }

    // сортируем
    sort(tasks1_sorted.begin(), tasks1_sorted.end());
    sort(tasks2_sorted.begin(), tasks2_sorted.end());

    // для отслеживания времени, когда освобождаются ...
    long long server_busy_time = 0;  // когда сервер сможет начать загрузку следующей задачи
    long long cpu1_busy_time = 0;      // ЦП1 освободится после выполнения текущей задачи
    long long cpu2_busy_time = 0;      // ЦП2

    int last_cpu = 0;     // 0 -- никто, ну и кто последний
    long long result = 0; // максимальное время окончания

    // итоговое расписание: процессор, номер задачи
    vector<pair<int, int>> timetable;
    timetable.reserve(n1 + n2);

    // i1 и i2 для задач каждого из ЦП
    int current_task1 = 0;
    int current_task2 = 0;

    // Основной цикл планирования загрузок задач на сервер и выполнение
    while (current_task1 < n1 || current_task2 < n2) { // пока есть задачи

        bool cpu1_has_tasks = (current_task1 < n1); // слишком часто дублировалось
        bool cpu2_has_tasks = (current_task2 < n2);

        int chosen_processor = 0; // куда будем загружать следующую задачу

        /*
        у нас было в условии, что сервер не может загружать задачу для процессора, когда он и так занят
        поэтому -- будем как бы чередовать, если есть задачи для них обоих

        когда я нарисовал примеры, то увидел явное чередование в загрузке
        и посчитал, что это необходимо для решения
        */
        if (last_cpu == 1) { // последний раз для ЦП1 загружали
            if (cpu2_has_tasks) { // если есть задачи для ЦП2
                chosen_processor = 2; // загружаем ему 
            } else if (cpu1_has_tasks) { // если там уже просто нет задач
                chosen_processor = 1; // надо завершить это дело для ЦП1
            }
        } else if (last_cpu == 2) { // аналогично, симметричнр
            if (cpu1_has_tasks) chosen_processor = 1;
            else if (cpu2_has_tasks) chosen_processor = 2;
        } else { // но а если мы в начальном состоянии
            // выбираем любой ЦП
            // точнее -- задачу с наивысшим приоритетом
            // как и в сортировке -- по p, потом по s, а потом уже и по id

            if (cpu1_has_tasks && cpu2_has_tasks) { // если и там и там есть задачи
                const auto& candidate1 = tasks1_sorted[current_task1]; // берем верхушки
                const auto& candidate2 = tasks2_sorted[current_task2];

                if (candidate1.p > candidate2.p) chosen_processor = 1; // и сравниваем их
                else if (candidate2.p > candidate1.p)chosen_processor = 2;
                else {
                    if (candidate1.s > candidate2.s) chosen_processor = 1;
                    else if (candidate2.s > candidate1.s) chosen_processor = 2;
                    else chosen_processor = 1; // если равны — выбираем процессор 1 по умолчанию
                } // иначе только один из них имеет
            } 
            else if (cpu1_has_tasks) chosen_processor = 1;// если только ЦП1 имеет -- то вариантов нет
            else if (cpu2_has_tasks) chosen_processor = 2;// ну и соответственно
        }

        // "симулируем" s и p
        if (chosen_processor == 1) { // если выбрали для 1
            Task& task_to_table = tasks1_sorted[current_task1];
            
            long long load_start_time = max(server_busy_time, cpu1_busy_time); // кто позже освободиться
            long long load_end_time = load_start_time + task_to_table.s; // загружаемся
            long long execute_end_time = load_end_time + task_to_table.p;   // исполняемся

            server_busy_time = load_end_time; // время заданятости сервера стало таким
            cpu1_busy_time = execute_end_time; // выполняется оно столько
            last_cpu = 1;
            result = max(result, execute_end_time);

            timetable.push_back({task_to_table.cpu_num, task_to_table.id}); // сохраняем в расписание
            current_task1++; // увеличиваем индекс
        } else if (chosen_processor == 2) { // аналогчино
            Task& task_to_table = tasks2_sorted[current_task2];

            long long load_start_time = max(server_busy_time, cpu2_busy_time);
            long long load_end_time = load_start_time + task_to_table.s;
            long long execute_end_time = load_end_time + task_to_table.p;

            server_busy_time = load_end_time;
            cpu2_busy_time = execute_end_time;
            last_cpu = 2;
            result = max(result, execute_end_time);

            timetable.push_back({task_to_table.cpu_num, task_to_table.id});
            current_task2++;
        } else 
            // больше задач нет
            break; 
    }

    //вывод
    out << result << "\n";
    for (const auto& p : timetable)
        out << p.first << " " << p.second << "\n";

    in.close(); out.close();
    return 0;
}
