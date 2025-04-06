#include <fstream>
#include <vector>
#include <string>
#include <iostream>
#include <stack>

using namespace std;

/*
идея состоит в том, чтобы 
матрица 26х26 -- сколько букв в английском лацинском алфавите
мы рассмотрим ориентированный граф, где 
начало слова --> конец слова
его рёбра  -- слова
ну если слова будут повторяться, то это будет массив из слов

нам нужно пройтись по всем словам И один раз +
+ нам важно, чтобы Самое первое слово начиналось с той буквы
на которую Закачнчивается последнее -- это Эйлеров цикл
    - кол вх и вых рёбер должно быть одинаковым для кажой веришны
    - граф сильно связанный
        обход в глубину
    - и нам все равно, повторяются ли рёбра

если всё хорошо -- обходим и формируем ответ
*/
int getAlphabetIndex(const char& c);
bool isAmoutInAndOutEquals(const vector<int>& in, const vector<int>& out);


void dfs(int vertex, const vector<vector<int>>& graph, vector<bool>& visited_vertexes);
bool dfsAndVisitedOneWay(int graph_start, const vector<vector<int>>& graph, const vector<int>& in, const vector<int>& out);
int getStartOfDetour(const vector<vector<int>>& graph);
bool isStronglyConnected(int& graph_start, const vector<vector<int>>& graph,
    const vector<vector<int>>& graph_transposed, const vector<int>& in, const vector<int>& out);

vector<int> getWinConbination(int n, int graph_start, vector<vector<int>>& graph);
void printWinConbination(vector<int>& win_conbination, vector<vector<string>>& words, ofstream& out);

int main() {
    std::ifstream in("input.txt");
    std::ofstream out("output.txt");
     
    int n; in >> n;

    /*
    матрица смежности будет хранить в себе "сколько рёбер есть из i в j"
    и отдельный массив под слова
    так же для проверки первого условия Эйнлера -- массивы на кол вход и выход
    */

    // 3д массив -- это круто, но он огромный будет
    vector<vector<string>> words((26 * 25 + 25) + 1); // как хранить слова (1)
    
    vector<int> in_amount(26,0); // сколько входят в вершину
    vector<int> out_amount(26,0); // сколько выходят

    vector<vector<int>> graph(26, vector<int>(26,0)); // матрица смежности
    vector<vector<int>> graph_transposed(26, vector<int>(26,0)); // её транспонированная версия

    // читаем слова
    // переменные, что будут учавствовать в цикле
    string word;
    int begin_of_word, end_of_word;
    int index;

    for (int i = 0; i < n; ++i) {
        //как я и говорил, будем 
        in >> word;
        begin_of_word = getAlphabetIndex(word[0]) - 1 ; // 0...25
        end_of_word = getAlphabetIndex(word[word.size() -  1]) - 1;//0...25
        
        // записываем слово
        index = 26 * begin_of_word + end_of_word;
        words[index].push_back(word);

        // обновляем сколько в кого вошло и вышло
        in_amount[end_of_word]++;
        out_amount[begin_of_word]++;

        // записываем в матрицу смежности Сколько слов у неё на дуге
        graph[begin_of_word][end_of_word]++;
        graph_transposed[end_of_word][begin_of_word]++;
    }

    // далее проверяем на цикл Эйлера
    // проверим первое условие -- количество входящих и вых должно совпадать
    bool is_amount_of_in_and_out_equals = isAmoutInAndOutEquals(in_amount, out_amount);
    // второе -- сильно ли связан
    int graph_start = getStartOfDetour(graph); // это как бы само начало (не обязательно всё начнётся с буквы А, например)
    bool is_strongly_connected = isStronglyConnected(graph_start, graph, graph_transposed, in_amount, out_amount);

    if (is_strongly_connected && is_amount_of_in_and_out_equals) { // если выполняются два условия на цикл Эйлера
        out << "Yes\n";
        vector<int> win_conbination = getWinConbination(n, graph_start, graph);// тут будем хранить нашу выигрышную комбинацию
        // но -- он будет в обратном порядке, поэтому -- надо учесть при выводе
        printWinConbination(win_conbination, words, out);
    } else {
        out << "No";
    }

    in.close(); out.close();
    return 0;
}

int getAlphabetIndex(const char& c) { // 1...26
    return (int) c - 96;
}

bool isAmoutInAndOutEquals(const vector<int>& in, const vector<int>& out) {
    for (int i = 0; i < 26; ++i) 
        if (in[i] != out[i])
            return false;
    return true;
}

bool isStronglyConnected(int& graph_start, const vector<vector<int>>& graph,
        const vector<vector<int>>& graph_transposed, const vector<int>& in, const vector<int>& out
) {
    vector<bool> visited_vertexes(26, false); // посещена ли буква под индексом index

    return dfsAndVisitedOneWay(graph_start, graph, in, out) && dfsAndVisitedOneWay(graph_start, graph_transposed, in, out);
}
int getStartOfDetour(const vector<vector<int>>& graph) {
    for (int i = 0; i < 26; ++i) {
        for (int j = 0; j < 26; ++j) {
            if (graph[i][j] > 0) {
                return i;
            }
        }
    }
    return 0; 
}
bool dfsAndVisitedOneWay(int graph_start, const vector<vector<int>>& graph, const vector<int>& in, const vector<int>& out) {
        vector<bool> visited_vertexes(26, false);
        dfs(graph_start, graph, visited_vertexes);
        for (int i = 0; i < 26; ++i) 
            if (visited_vertexes[i] == false && // если вершина была не посещена
                (in[i] > 0 || out[i] > 0)) // И  если есть что-то вх или вых (2)
                return false;
        return true;
    }

// общая задача у меня со стеком, тут хотя бы так посмотрим
void dfs(int vertex, const vector<vector<int>>& graph, vector<bool>& visited_vertexes) {
    visited_vertexes[vertex] = true; // да -- мы тут и оказались
    for (int i = 0; i < 26; ++i) // обходим все смежные вершины 
        if (graph[vertex][i] != 0 && !visited_vertexes[i])
            dfs(i, graph, visited_vertexes); 
}

vector<int> getWinConbination(int n, int graph_start, vector<vector<int>>& graph) {
    vector<int> win_conbination;
    stack<int> stack_; stack_.push(graph_start);
    while(!stack_.empty()) {
        int vertex = stack_.top();
        int index;
        for (index = 0; index < 26; ++index) // ищем первое ребро, что соед Vertex и Index
            if (graph[vertex][index]) 
                break;
        if (index == 26) { // если его нет
            win_conbination.push_back(vertex); // то это конец, поэтому у нас массив в обратном порядке строиться
            stack_.pop(); // и удаляем из стека
        } else {
            --graph[vertex][index]; // посетили, т.е. -- как бы ребро удалили
            stack_.push(index); // теперь мы там в глубину зашли и идёт на вершину стека
        }
    } 
    return win_conbination;
}

void printWinConbination(vector<int>& win_conbination, vector<vector<string>>& words, ofstream& out) {
    // (3)
    int temp; int last;
    for (
        int i = win_conbination.size() - 1;
        i >= 1;
        --i
    ) {
        temp = 26 * (win_conbination[i]) + (win_conbination[i - 1]);
        last = words[temp].size() - 1;
        out << words[temp][last] << "\n";
        words[temp].erase(words[temp].begin() + last); // с конца удалить быстрее
    }
}


// (1)
/*
    a и b -- начало и конец слова соответсвенно
    a и b лежат в диапазоне от 1 до 26
    что мы делаем
    разбиваем массив на 26 блоков (характерезует каждую букву алфавита)
    и сам блок на 26 блоков
    т.е. 26 на 26 ячеек в массиве
    26 * (a - 1) + b -- в этой ячейке хрантиться vector<string>, в котором лежат слова начинающиеся на a и заканчивающиеся на b
*/

// (2)
/*
    if (visited_vertexes[i] == false && // если вершина была не посещена
            (in[i] > 0 || out[i] > 0)) // И  если есть что-то вх или вых (2)

    почему так
    начну с того, что visited_vertexes у нас представляет все вершины
    и может быть ситуация, когда буквы как таковой вообще нет
    поэтому второе условие показывает, что Вершина вообще существует
*/

// (3) 
/*
    как выглядит win_conbination.reversed();
    вот возьмём исходный пример
    c t r d t e c' (c' -- начало)
    и мы смотрим вот так
    c t
      t r
        r d 
          ...
*/