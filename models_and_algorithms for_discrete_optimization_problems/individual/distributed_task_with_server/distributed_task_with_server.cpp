#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <unordered_set>
#include <algorithm>
#include <climits>

using namespace std;

struct Task {
    int id;
    long executing_time;
    long loading_time;
};

struct StateKey {
    int i1;
    int i2;
    int prevProc;

    bool operator==(const StateKey& other) const {
        return i1 == other.i1 && i2 == other.i2 && prevProc == other.prevProc;
    }
};

namespace std {
    template<> struct hash<StateKey> {
        size_t operator()(const StateKey& k) const {
            return hash<int>()(k.i1) ^ hash<int>()(k.i2) ^ hash<int>()(k.prevProc);
        }
    };
}

struct State { // описывает состояние
    int i1; // индексы следующих задач для ЦП1
    int i2; // ЦП2
    long server_busy_time; // время освобождения Сервера
    long cpu1_busy_time; // время завершения последней задачи на ЦП1
    long cpu2_busy_time; // ЦП2
    int prevProc; // последний использованный Процессор (0, 1, 2)
    long cost; // current max время завершения
};

struct TimetableStep {
    int cpu;
    int task_id;
};

struct StateRecord {
    long f;
    State current_state;
    vector<TimetableStep> path;

    bool operator<(const StateRecord& other) const {
        return f > other.f; // For min-heap
    }
};

long heuristic(int i1, int i2, const vector<Task>& t1, const vector<Task>& t2) {
    long sum = 0;
    for (int i = i1; i < t1.size(); i++) {
        sum += t1[i].loading_time + t1[i].executing_time;
    }
    for (int i = i2; i < t2.size(); i++) {
        sum += t2[i].loading_time + t2[i].executing_time;
    }
    return sum / 2;
}

int main() {
    ifstream in("input.txt");
    ofstream out("output.txt");

    // считываем данные
    int n1, n2;
    in >> n1 >> n2;

    vector<Task> tasks1(n1);
    vector<Task> tasks2(n2);

    for (int i = 0; i < n1; i++) {
        in >> tasks1[i].executing_time >> tasks1[i].loading_time;
        tasks1[i].id = i + 1; // хранить будем не от 0,а от 1
    }

    for (int i = 0; i < n2; i++) {
        in >> tasks2[i].executing_time >> tasks2[i].loading_time;
        tasks2[i].id = i + 1;
    }

    priority_queue<StateRecord> pq;
    unordered_set<StateKey> visited;

    State initial_state = {0, 0, 0, 0, 0, 0, 0};
    pq.push({0, initial_state, {}});

    while (!pq.empty()) {
        StateRecord current = pq.top();
        pq.pop();

        StateKey key = {current.current_state.i1, current.current_state.i2, current.current_state.prevProc};
        if (visited.count(key)) continue;
        visited.insert(key);

        if (current.current_state.i1 == n1 && current.current_state.i2 == n2) {
            long makespan = max(current.current_state.cpu1_busy_time, current.current_state.cpu2_busy_time);
            out << makespan << "\n";
            for (const auto& step : current.path) {
                out << step.cpu << " " << step.task_id << "\n";
            }
            out.close();
            return 0;
        }

        // Попытка загрузить задачу на процессор 1
        if (current.current_state.i1 < n1 && current.current_state.prevProc != 1) {
            Task t = tasks1[current.current_state.i1];
            long start = max(current.current_state.server_busy_time, current.current_state.cpu1_busy_time);
            long newServer = start + t.loading_time;
            long newProc1 = newServer + t.executing_time;
            long newCost = max(newProc1, current.current_state.cpu2_busy_time);

            State next = {
                current.current_state.i1 + 1,
                current.current_state.i2,
                newServer,
                newProc1,
                current.current_state.cpu2_busy_time,
                1,
                newCost
            };

            vector<TimetableStep> newPath = current.path;
            newPath.push_back({1, t.id});
            long h = heuristic(next.i1, next.i2, tasks1, tasks2);
            pq.push({next.cost + h, next, newPath});
        }

        // Попытка загрузить задачу на процессор 2
        if (current.current_state.i2 < n2 && current.current_state.prevProc != 2) {
            Task t = tasks2[current.current_state.i2];
            long start = max(current.current_state.server_busy_time, current.current_state.cpu2_busy_time);
            long newServer = start + t.loading_time;
            long newProc2 = newServer + t.executing_time;
            long newCost = max(newProc2, current.current_state.cpu1_busy_time);

            State next = {
                current.current_state.i1,
                current.current_state.i2 + 1,
                newServer,
                current.current_state.cpu1_busy_time,
                newProc2,
                2,
                newCost
            };

            vector<TimetableStep> newPath = current.path;
            newPath.push_back({2, t.id});
            long h = heuristic(next.i1, next.i2, tasks1, tasks2);
            pq.push({next.cost + h, next, newPath});
        }
    }

    out << "-1\n"; // unreachable (should be impossible)
    out.close();

    return 0;
}