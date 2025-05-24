#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <unordered_set>
#include <algorithm>
#include <climits>

using namespace std;

struct Task {
    int id; // номер задачи
    long p; // время выполнения
    long s; // загрузки на сервер
};

struct StateKey {
    int next_task_for_1_index; // индекс следующей задачи для процессора 1
    int next_task_for_2_index; // индекс следующей задачи для процессора 2
    int last_cpu; // последний использованный CPU

    StateKey(int task1, int task2, int cpu): next_task_for_1_index(task1), next_task_for_2_index(task2), last_cpu(cpu) {}


    bool operator==(const StateKey& other) const {
        return next_task_for_1_index == other.next_task_for_1_index && next_task_for_2_index == other.next_task_for_2_index && last_cpu == other.last_cpu;
    }
};

// для того, чтобы работала с unordered_set
namespace std {
    template<> struct hash<StateKey> {
        size_t operator()(const StateKey& k) const {
            return hash<int>()(k.next_task_for_1_index) ^ hash<int>()(k.next_task_for_2_index) ^ hash<int>()(k.last_cpu);
        }
    };
}

struct State {
    int next_task_for_1_index; // индекс следующей задачи для процессора 1
    int next_task_for_2_index; // индекс следующей задачи для процессора 2
    long server_busy_time; // когда освободиться сервер
    long cpu1_busy_time; // когда освободиться ЦП1
    long cpu2_busy_time; // когда освободиться ЦП2
    int last_cpu; // последний использованный процессор
    long cost; // current max время завершения

    State(): next_task_for_1_index(0), next_task_for_2_index(0), server_busy_time(0), cpu1_busy_time(0), cpu2_busy_time(0), last_cpu(0), cost(0) {}

    State(int next_task_for_1_index, int next_task_for_2_index, long server_busy_time, long cpu1_busy_time, long cpu2_busy_time,int last_cpu,long cost): 
        next_task_for_1_index(next_task_for_1_index), 
        next_task_for_2_index(next_task_for_2_index),
        server_busy_time(server_busy_time), 
        cpu1_busy_time(cpu1_busy_time), 
        cpu2_busy_time(cpu2_busy_time), 
        last_cpu(last_cpu), 
        cost(cost) {}
};

struct TimetableStep { // единица расписания
    int cpu_index; // индекс процессора
    int task_id; // задача на процессоре
};

struct StateRecord {
    long f; // cost + heurestic
    State current_state; // текущее состояние
    vector<TimetableStep> path; // загруженные задачи

    bool operator<(const StateRecord& other) const {
        return f > other.f;
    }
};

class TaskTimetable {
private:
    vector<Task> tasks1;
    vector<Task> tasks2;
    
    void readInput(ifstream& in) {
        int n1, n2;
        in >> n1 >> n2;

        tasks1.resize(n1);
        tasks2.resize(n2);

        for (int i = 0; i < n1; i++) {
            in >> tasks1[i].p >> tasks1[i].s;
            tasks1[i].id = i + 1; // для нумерации с 1
        }

        for (int i = 0; i < n2; i++) {
            in >> tasks2[i].p >> tasks2[i].s;
            tasks2[i].id = i + 1;
        }
    }

    long heuristic(int next_task_for_1_index, int next_task_for_2_index) const {
        long max1 = 0, max2 = 0;
        for (int i = next_task_for_1_index; i < tasks1.size(); ++i)
            max1 += tasks1[i].s + tasks1[i].p;
        for (int i = next_task_for_2_index; i < tasks2.size(); ++i)
            max2 += tasks2[i].s + tasks2[i].p;
        return max(max1, max2); // как будто паралельно, но займет времени максимум из одного процессоров
    }


    void process_cpu(priority_queue<StateRecord>& pq, const StateRecord& current, int cpu_num) {
        // Выбираем нужные данные в зависимости от процессора
        bool has_tasks = (cpu_num == 1) 
            ? (current.current_state.next_task_for_1_index < tasks1.size())
            : (current.current_state.next_task_for_2_index < tasks2.size());
        
        if ( // если
            has_tasks // есть задачи
            && 
            current.current_state.last_cpu != cpu_num // сохраняется чередование
        ) {
            const Task& next_task = (cpu_num == 1) // берем следующую задачу
                ? tasks1[current.current_state.next_task_for_1_index]
                : tasks2[current.current_state.next_task_for_2_index];
            
            long cpu_busy_time = (cpu_num == 1)
                ? current.current_state.cpu1_busy_time
                : current.current_state.cpu2_busy_time;
            
          
            long start_time = max(current.current_state.server_busy_time, cpu_busy_time); // время старта: кто позже освободиться -- шина или процессор
            long next_time_server = start_time + next_task.s; // (начало + загрузка) + вычисление  
            long new_time_cpu = next_time_server + next_task.p; // (начало + загрузка) + вычисление
        
            long other_cpu_time = (cpu_num == 1)
                ? current.current_state.cpu2_busy_time
                : current.current_state.cpu1_busy_time;
            long new_cost = max(new_time_cpu, other_cpu_time); // новое общее время

    
            State next = State( // создаем новое состояние
                (cpu_num == 1) ? current.current_state.next_task_for_1_index + 1 : current.current_state.next_task_for_1_index, // сдвигаем на след задачу (если мы на cpu1)
                (cpu_num == 2) ? current.current_state.next_task_for_2_index + 1 : current.current_state.next_task_for_2_index, // сдвигаем на след задачу (если мы на cpu2)
                next_time_server, // новое время на сервере
                (cpu_num == 1) ? new_time_cpu : current.current_state.cpu1_busy_time, // и для ЦП1 (если мы на cpu1)
                (cpu_num == 2) ? new_time_cpu : current.current_state.cpu2_busy_time, // и для ЦП2 (если мы на cpu2)
                cpu_num, // кто был последний
                new_cost
            );

            // обновляем
            vector<TimetableStep> new_path = current.path;
            new_path.push_back({cpu_num, next_task.id});

            // добовляем в очередь
            pq.push({next.cost + heuristic(next.next_task_for_1_index, next.next_task_for_2_index), 
                    next, new_path});
        }
    }
    
    void process_cpu1(priority_queue<StateRecord>& pq, const StateRecord& current) {
        process_cpu(pq, current, 1);
    }

    void process_cpu2(priority_queue<StateRecord>& pq, const StateRecord& current) {
        process_cpu(pq, current, 2);
    }

    void writeOutput(ofstream& out, const StateRecord& solution) const {
        long makespan = max(solution.current_state.cpu1_busy_time, solution.current_state.cpu2_busy_time);
        out << makespan << "\n";
        for (const auto& step : solution.path) {
            out << step.cpu_index << " " << step.task_id << "\n";
        }
    }

public:
    void solve(const string& inputFile, const string& outputFile) {
        ifstream in(inputFile);
        ofstream out(outputFile);
        readInput(in);

        priority_queue<StateRecord> pq;
        unordered_set<StateKey> visited;

        State initial_state = State(); // начальное состояние
        pq.push({0, initial_state, {}});

        while (!pq.empty()) { // пока очередь не опустела
            StateRecord current = pq.top(); pq.pop(); // извлекаем текущее состояние (с минимальной оценкой)

            // проверяем на посещение
            StateKey key = StateKey(
                current.current_state.next_task_for_1_index, 
                current.current_state.next_task_for_2_index,
                current.current_state.last_cpu
            );

            // если уже обработанно 
            if (visited.count(StateKey(current.current_state.next_task_for_1_index, current.current_state.next_task_for_2_index,current.current_state.last_cpu))) 
                continue; // продолжаем
            else 
                visited.insert(key); // иначе посещаем

            // если все задачи просчитаны, то -- заканчиваем
            if (current.current_state.next_task_for_1_index == tasks1.size() && current.current_state.next_task_for_2_index == tasks2.size()) {
                writeOutput(out, current);
                out.close();
                return;
            }

            // "пробуем" загрузить на один из процессоров
            process_cpu1(pq, current); 
            process_cpu2(pq, current);
        }
        out.close();
    }
};

int main() {
    TaskTimetable timetable;
    timetable.solve("input.txt", "output.txt");
    return 0;
}