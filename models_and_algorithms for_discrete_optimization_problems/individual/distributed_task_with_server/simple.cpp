#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <tuple>
#include <algorithm>

using namespace std;

struct Task {
    int id;
    long p, s;
};

int main() {
    ifstream in("input.txt");
    ofstream out("output.txt");

    int n1, n2;
    in >> n1 >> n2;

    vector<Task> tasks1(n1), tasks2(n2);

    for (int i = 0; i < n1; ++i) {
        in >> tasks1[i].p >> tasks1[i].s;
        tasks1[i].id = i + 1;
    }
    for (int i = 0; i < n2; ++i) {
        in >> tasks2[i].p >> tasks2[i].s;
        tasks2[i].id = i + 1;
    }

    int i1 = 0, i2 = 0;
    long server_time = 0, cpu1_time = 0, cpu2_time = 0;
    int last_cpu = 0;

    vector<pair<int, int>> order;

    while (i1 < n1 || i2 < n2) {
        // доступные задачи
        vector<tuple<long, int, int>> candidates; // {время завершения, процессор, индекс задачи}

        if (i1 < n1 && last_cpu != 1) {
            long start = max(server_time, cpu1_time);
            long finish = start + tasks1[i1].s + tasks1[i1].p;
            candidates.push_back({finish, 1, i1});
        }
        if (i2 < n2 && last_cpu != 2) {
            long start = max(server_time, cpu2_time);
            long finish = start + tasks2[i2].s + tasks2[i2].p;
            candidates.push_back({finish, 2, i2});
        }

        if (candidates.empty()) {
            // нужно подождать, если нет подходящей задачи
            server_time = min(cpu1_time, cpu2_time);
            last_cpu = 0; // можно будет загрузить любую
            continue;
        }

        // выбираем наименьшее завершение
        auto [finish, cpu, idx] = *min_element(candidates.begin(), candidates.end());

        if (cpu == 1) {
            long start = max(server_time, cpu1_time);
            server_time = start + tasks1[i1].s;
            cpu1_time = server_time + tasks1[i1].p;
            order.emplace_back(1, tasks1[i1].id);
            i1++;
            last_cpu = 1;
        } else {
            long start = max(server_time, cpu2_time);
            server_time = start + tasks2[i2].s;
            cpu2_time = server_time + tasks2[i2].p;
            order.emplace_back(2, tasks2[i2].id);
            i2++;
            last_cpu = 2;
        }
    }

    long total_time = max(cpu1_time, cpu2_time);
    cout << total_time << "\n";
    for (auto [cpu, id] : order)
        cout << cpu << " " << id << "\n";

    return 0;
}
