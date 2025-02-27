#include <fstream>
#include <vector>
#include <stack>

class BinaryTree
{
private:
    struct Node {
        int data;
        Node* left;
        Node* right;
        Node(int data) : data(data), left(nullptr), right(nullptr) {}
        Node() : data(0), left(nullptr), right(nullptr){}
        void setData(int data) {this->data = data;}
        int getData() const {return this->data;}
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
public:
    Node* root;
    BinaryTree() {
        this->root = nullptr;
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
    };

    BinaryTreeIterator getBinaryTreeInterator() {
        return BinaryTreeIterator(this->root);
    }

    void delete_node(int data) {
        this->root = this->delete_node_helper(this->root, data);
    }

};

int main() {
    std::ifstream in("input.txt");
    std::ofstream out("output.txt");

    BinaryTree tree;
    int need_to_delete;
    in >> need_to_delete;
    
    int temp;
    while (in >> temp) {
        tree.insert(temp);
    }
    
    tree.delete_node(need_to_delete);

    BinaryTree::BinaryTreeIterator it = tree.getBinaryTreeInterator();
    while (!it.isDone()) {
        out << it.getCurrentNodeData() << "\n";
        it.next();
    }

    in.close(); out.close();
    return 0;
}