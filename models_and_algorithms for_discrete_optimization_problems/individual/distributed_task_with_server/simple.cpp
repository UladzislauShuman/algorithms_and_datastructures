#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <tuple>
#include <algorithm>

using namespace std;

/*
вот незадача -- не проходит два теста
и с этим нужно что-то делать
я могу:
    - посчитать двумя алгоритмами и выбрать выгоднейший
    - как-то изменить эвристику
    - в нынешнее решение добавть "просмотр в будущее"

*/

struct Task {
    int id;
    long p;
    long s;
};

int main() {
    ifstream in("input.txt");
    ofstream out("output.txt");

    // считываем
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

    int i_cpu1 = 0; // индекс по tasks1
    int i_cpu2 = 0; // индекс по tasks2
    long server_busy_time = 0; // время, ДО которого занят сервер
    long cpu1_busy_time = 0; // ЦП1
    long cpu2_busy_time = 0; // ЦП2
    int last_cpu = 0; // последний процессор, которому загружалась задача

    vector<pair<int, int>> tasks_order;

    while ( // пока
        i_cpu1 < n1 || // не выполнены все задачи в tasks1 
        i_cpu2 < n2 // или tasks2
    ) {
        // доступные задачи
        vector<tuple<long, int, int>> candidates; // {время завершения, процессор, индекс задачи}

        
        if ( // если 
            i_cpu1 < n1 // мы еще не закончили с tasks1
            //&& last_cpu != 1 // И чередуемся
        ) {
            long start = max(server_busy_time, cpu1_busy_time); // можно, когда максимум из 
            long finish_time = start + tasks1[i_cpu1].s + tasks1[i_cpu1].p; // начало + весь цикл загрузки
            candidates.push_back({finish_time, 1, i_cpu1}); 
        }
        if (i_cpu2 < n2 
            // && last_cpu != 2
        ) { // аналогично для ЦП2
            long start = max(server_busy_time, cpu2_busy_time);
            long finish_time = start + tasks2[i_cpu2].s + tasks2[i_cpu2].p;
            candidates.push_back({finish_time, 2, i_cpu2});
        }

        if (candidates.empty()) { // нет задач, которые можно загрузить
            // нужно подождать, если нет подходящей задачи
            server_busy_time = max(server_busy_time, min(cpu1_busy_time, cpu2_busy_time));
            last_cpu = 0; // как бы чередование в момент этот не важно
            continue;
        }

        // выбираем задачу по наименьшему времени завершения
        auto [finish_time, cpu, idx] = *min_element(candidates.begin(), candidates.end(),
                [](const auto &a, const auto &b) {
                    return tie(get<0>(a), get<1>(a), get<2>(a)) < tie(get<0>(b), get<1>(b), get<2>(b));
                });



        if (cpu == 1) { // загружаем ЦП1
            long start = max(server_busy_time, cpu1_busy_time);
            server_busy_time = start + tasks1[i_cpu1].s;
            cpu1_busy_time = server_busy_time + tasks1[i_cpu1].p;
            tasks_order.emplace_back(1, tasks1[i_cpu1].id); // загрузили и зафиксировали это
            i_cpu1++;
            last_cpu = 1;
        } else { // аналогично
            long start = max(server_busy_time, cpu2_busy_time);
            server_busy_time = start + tasks2[i_cpu2].s;
            cpu2_busy_time = server_busy_time + tasks2[i_cpu2].p;
            tasks_order.emplace_back(2, tasks2[i_cpu2].id);
            i_cpu2++;
            last_cpu = 2;
        }
    }
    
    /*
    всю прогармму у нас эти два показателя увеличивались
    и берем самый макс -- пока не закончит работу Весь Сервер
    */
    long total_time = max(cpu1_busy_time, cpu2_busy_time);

    cout << total_time << "\n";
    for (auto [cpu, id] : tasks_order)
        cout << cpu << " " << id << "\n";

    in.close(); out.close();
    return 0;
}
