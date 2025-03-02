#include <fstream>
#include <vector>
#include <stack>
#include <string>
#include <algorithm>
#include <iostream>

class BinaryTree
{
private:
    struct Node {
        int data;
        Node* left;
        Node* right;
        int descendantsAmount;
        Node(int data) : data(data), left(nullptr), right(nullptr), descendantsAmount(0){}
        Node() : data(0), left(nullptr), right(nullptr), descendantsAmount(0){}
        void setData(int data) {this->data = data;}
        int getData() const {return this->data;}

        std::string toString() const {return "Node " + std::to_string(this->data) + " " + std::to_string(this->descendantsAmount);}
    };

    void insertHelper(Node* node, int data) {
        if (node->getData() == data) 
            return;
        if (data < node->getData()) {
            if (node->left == nullptr) {
                node->left = new Node(data);
                return;
            }
            else 
                this->insertHelper(node->left, data);
        }
        if (data > node->getData()) {
            if (node->right == nullptr){
                node->right = new Node(data);
                return;
            }
            else
                this->insertHelper(node->right, data);
        }
    }  

    Node* find_min(Node* node) {
        while (node->left != nullptr) node = node->left;
        return node;
    }

    Node* delete_node_helper(Node* node, int data) { // рекурсивно работает
    // возвращаемое как бы играет роль привязки для предыдущего
    //  тут как бы поиск
    // оно не выйдет за пределы 
        if (node == nullptr) return node; 
        if (data < node->getData()) {
            node->left = delete_node_helper(node->left, data);
            return node;
        }
        if (data > node->getData()) {
            node->right = delete_node_helper(node->right, data);
            return node;
        }
        // если мы оказались в этой точке, то зн что data == node->getData()
        // прешло время удалять
        
        
        // если только один потомок
        /*
            если слева потомка нет, то просто правый ставим на место удаляемого
        */
        if (node->left == nullptr) {
            Node* temp = node->right; 
            delete node;
            return temp;
        }
        // если мы тут, то -- то у нас есть левый потомок 
        /*
            если справа потомка нет, то просто левый ставим на место удаляемого
        */
        if (node->right == nullptr) {
            Node* temp = node->left;
            delete node;
            return temp;
        }
        // если мы уже и тут оказались, то у нас есть два потомка
        // для правого удаления нам надо вправо И потом в самое лево
        Node* temp = find_min(node->right); // т.е. ищем минимальный (самый левый) для правого детёныша
        node->setData(temp->getData()); // мы не будем как бы менять адреса -- мы просто поменяем менями значения И find_min 
        node->right = delete_node_helper(node->right, temp->getData()); // удаляем тот самый find_min
        return node;
    }

    void deleteBinaryTree(Node* node) {
        if (node) {
            deleteBinaryTree(node->left);
            deleteBinaryTree(node->right);
            delete node;
        }
    }

    // часть индивидуального задания
    int getHeightHelper(Node* node) {
        if (node == nullptr) {
            return -1;
        }
    
        int leftHeight = getHeightHelper(node->left);
        int rightHeight = getHeightHelper(node->right);
    
        // Вычисляем количество потомков для текущего узла:
        int leftDescendants = (node->left == nullptr) ? 0 : node->left->descendantsAmount + 1;
        int rightDescendants = (node->right == nullptr) ? 0 : node->right->descendantsAmount + 1;
        node->descendantsAmount = leftDescendants + rightDescendants;
    
        return 1 + std::max(leftHeight, rightHeight);
    }
    
    std::vector<Node*> nodes_on_half_of_height;
    void collect_nodes_on_half_of_height_helper(Node* node, int height_) {
        if (node == nullptr) return;
        if (height_ == this->height >> 1 ) {
            if (node->left) {// если он вообще существует, иначе нам не интересно
                if (!node->right || node->left->descendantsAmount > node->right->descendantsAmount) {
                    this->nodes_on_half_of_height.push_back(node);
                }
            }
        } else {
            if (node->left) collect_nodes_on_half_of_height_helper(node->left, height_ - 1);
            if (node->right) collect_nodes_on_half_of_height_helper(node->right, height_ - 1);
        }
    }
public:
    int height;
    Node* root;
    BinaryTree() {
        this->root = nullptr;
        this->height = 0;
    }
    ~BinaryTree() {
        this->deleteBinaryTree(this->root);
    }

    void insert(int data)
    {
        if (root == nullptr) {
            root = new Node(data);
        } else {
            insertHelper(root, data);
        }
    }

    // реализация паттерна Итератор для Прямого Левого Обхода бин-дерева
    class BinaryTreeIterator {
    private:
        std::stack<Node*> nodeStack;  
        friend class BinaryTree;

        Node* getCurrentNode() {
            if (!nodeStack.empty()) {
                return nodeStack.top();
            }
            return nullptr;
        }
    public:
        BinaryTreeIterator(Node* root_)  {
            if (root_) nodeStack.push(root_);
        }
        // без метода first по банде-4
        void next() {
            if (!nodeStack.empty()) {
                Node* node = nodeStack.top();
                nodeStack.pop();
   
                if (node->right) {
                    nodeStack.push(node->right);
                }
                if (node->left) {
                    nodeStack.push(node->left);
                }
            }
        }
    
        bool isDone() {
            return nodeStack.empty();
        }
        int getCurrentNodeData() {
            if (!nodeStack.empty()) {
                return nodeStack.top()->getData();
            }
            return -1;
        }

        std::string toString() const {
            if (!nodeStack.empty()) {
                return nodeStack.top()->toString();
            }
            return "\n";
        }
    };

    BinaryTreeIterator getBinaryTreeInterator() {
        return BinaryTreeIterator(this->root);
    }

    void delete_node(int data) {
        this->root = this->delete_node_helper(this->root, data);
    }
    //часть индивидуального задания
    int getHeight() {
        this->height = this->getHeightHelper(this->root);
        return this->height;
    }

    void delete_element_for_task() {
        this->getHeight(); // дерево запомнило свою высоту И заполнило Node-ы о том, сколько у них потомков
        this->collect_nodes_on_half_of_height_helper(this->root, this->height); // собрали все нужны по условию Node-ы
        if (!this->nodes_on_half_of_height.empty()) {
            int medianIndex = (this->nodes_on_half_of_height.size() - 1) >> 1;
            std::nth_element(this->nodes_on_half_of_height.begin(), 
                            this->nodes_on_half_of_height.begin() + medianIndex, 
                            this->nodes_on_half_of_height.end(),
                             [](const Node* a, const Node* b) {
                                 return a->data < b->data;
                             });
            this->delete_node(this->nodes_on_half_of_height[medianIndex]->data);
        }        
    }
};

int main() {
 
    std::ifstream in("input8.txt");
    std::ofstream out("output.txt");

    BinaryTree tree;
    
    int temp;
    while (in >> temp) {
        tree.insert(temp);
    }
    tree.delete_element_for_task();
    BinaryTree::BinaryTreeIterator it = tree.getBinaryTreeInterator();
    while (!it.isDone()) {
        out << it.getCurrentNodeData() << "\n";
        it.next();
    }

    in.close(); out.close();
    return 0;
}