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
};

int main() {
    std::ifstream in("input.txt");
    std::ofstream out("output.txt");

    BinaryTree tree;
    int temp;
    while (in >> temp) {
        tree.insert(temp);
    }
    
    BinaryTree::BinaryTreeIterator it = tree.getBinaryTreeInterator();

    while (!it.isDone()) {
        out << it.getCurrentNodeData() << "\n";
        it.next();
    }

    in.close(); out.close();
    return 0;
}