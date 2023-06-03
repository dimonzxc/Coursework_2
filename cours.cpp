#include <iostream>
#include <iomanip>
#include <ctime>
#include <queue>
#include <windows.h>
#include <random>
#include <fstream>

#define output_tree "C:\\Users\\Дмитрий\\source\\repos\\cours\\output_tree.txt"
#define output_key "C:\\Users\\Дмитрий\\source\\repos\\cours\\output_key.txt"
#define output_ans "C:\\Users\\Дмитрий\\source\\repos\\cours\\output_ans.txt"
#define output_task "C:\\Users\\Дмитрий\\source\\repos\\cours\\output_task.txt"
#define input_tree "C:\\Users\\Дмитрий\\source\\repos\\cours\\rb_tree.txt"
bool isTreeCreated = false;
using namespace std;
enum ConsoleColor {
    BLACK = 0,
    RED = 4,
    WHITE = 15
};

void SetConColor(ConsoleColor text, ConsoleColor background) {
    HANDLE hConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsoleHandle, (WORD)((background << 4) | text));
}

struct Tree {
    int value;
    Tree* left;
    Tree* right;
    ConsoleColor color;
};
bool isRed(Tree* node) {
    return (node != nullptr && node->color == ConsoleColor::RED);
}

// Функция для перекраски узлов
void recolor(Tree* node) {
    node->color = ConsoleColor::RED;
    node->left->color = ConsoleColor::BLACK;
    node->right->color = ConsoleColor::BLACK;
}

// Функция для поворота влево
Tree* rotateLeft(Tree* node) {
    Tree* child = node->right;
    node->right = child->left;
    child->left = node;
    child->color = node->color;
    node->color = ConsoleColor::RED;
    return child;
}

// Функция для поворота вправо
Tree* rotateRight(Tree* node) {
    Tree* child = node->left;
    node->left = child->right;
    child->right = node;
    child->color = node->color;
    node->color = ConsoleColor::RED;
    return child;
}

// Функция для балансировки красно-черного дерева
Tree* balanceTree(Tree* node) {
    if (isRed(node->right) && !isRed(node->left)) {
        node = rotateLeft(node);
    }
    if (isRed(node->left) && isRed(node->left->left)) {
        node = rotateRight(node);
    }
    if (isRed(node->left) && isRed(node->right)) {
        recolor(node);
    }
    return node;
}
int SearchElement(int key, Tree* tree) {
    if (tree != NULL) {
        if (key == tree->value) {
            cout << "Element found in the tree. ";
            return 0;
        }
        else if (key < tree->value) {
            return SearchElement(key, tree->left);
        }
        else {
            return SearchElement(key, tree->right);
        }
    }
    else {
        cout << "Element not found in the tree. ";
        return 0;
    }
}

Tree* DeleteElement(int key, Tree* tree, bool isConsole) {
    if (tree == NULL) {
        if (isConsole) {
            cout << "Element not found in the tree. ";
        }
        else {
            ofstream File(output_key, ios::app);
            File << "Element not found in the tree." << '\n';
            File.close();
        }
        return tree;
    }
    else if (key == tree->value) {
        if (isConsole) {
            cout << "Element found in the tree. ";
        }
        else {
            ofstream File(output_key, ios::app);
            File << "Element found in the tree." << '\n';
            File.close();
        }

        Tree* tmp;
        if (tree->right == NULL)
            tmp = tree->left;
        else {
            Tree* ptr = tree->right;
            if (ptr->left == NULL) {
                ptr->left = tree->left;
                tmp = ptr;
            }
            else {
                Tree* ppmin = ptr->left;
                while (ppmin->left != NULL) {
                    ptr = ppmin;
                    ppmin = ptr->left;
                }
                ptr->left = ppmin->right;
                ppmin->left = tree->left;
                ppmin->right = tree->right;
                tmp = ppmin;
            }
        }
        delete tree;
        return tmp;
    }
    else if (key < tree->value) {
        tree->left = DeleteElement(key, tree->left, isConsole);
    }
    else {
        tree->right = DeleteElement(key, tree->right, isConsole);
    }
    return tree;
}

Tree* CreateNode(int value, ConsoleColor color) {
    Tree* tree = new Tree;
    tree->value = value;
    tree->left = tree->right = nullptr;
    tree->color = color;
    return tree;

}

Tree* InsertElement(int key, Tree* tree, bool isConsole) {
    if (!isConsole) {
        ofstream File(output_key, ios::app);
        File << key << " вставлен в дерево" << '\n';
        File.close();
    }
    if (!isTreeCreated) {
        // Вставка элемента в пустое дерево
        isTreeCreated = true;
        return CreateNode(key, RED);
    }

    if (key < tree->value) {
        if (tree->left) {
            // Вставка в левое поддерево
            tree->left = InsertElement(key, tree->left, 1);
        }
        else {
            tree->left = CreateNode(key, BLACK);
        }
    }
    else if (key > tree->value) {
        if (tree->right) {
            // Вставка в правое поддерево
            tree->right = InsertElement(key, tree->right, 1);
        }
        else {
            tree->right = CreateNode(key, BLACK);
        }
    }
    else {
        // Дублирующийся ключ, ничего не делаем
        return tree;
    }
    // Проверка и исправление нарушений свойств красно-черного дерева
    if (tree->left && tree->left->color == RED && tree->left->left && tree->left->left->color == RED) {
        // LL нарушение - перекрашиваем узлы
        Tree* leftChild = tree->left;
        tree->left = leftChild->right;
        leftChild->right = tree;
        leftChild->color = BLACK;
        tree->color = RED;
        return leftChild;
    }
    else if (tree->left && tree->left->color == RED && tree->left->right && tree->left->right->color == RED) {
        // LR нарушение - поворачиваем и перекрашиваем узлы
        Tree* leftChild = tree->left;
        tree->left = leftChild->right;
        leftChild->right = tree->left->left;
        tree->left->left = leftChild;
        leftChild->color = BLACK;
        tree->color = RED;
        return tree->left;
    }
    else if (tree->right && tree->right->color == RED && tree->right->left && tree->right->left->color == RED) {
        // RL нарушение - поворачиваем и перекрашиваем узлы
        Tree* rightChild = tree->right;
        tree->right = rightChild->left;
        rightChild->left = tree->right->right;
        tree->right->right = rightChild;
        rightChild->color = BLACK;
        tree->color = RED;
        return tree->right;
    }
    else if (tree->right && tree->right->color == RED && tree->right->right && tree->right->right->color == RED) {
        // RR нарушение - перекрашиваем узлы
        Tree* rightChild = tree->right;
        tree->right = rightChild->left;
        rightChild->left = tree;
        rightChild->color = BLACK;
        tree->color = RED;
        return rightChild;
    }

    return balanceTree(tree);
}

Tree* DestroyTree(Tree* tree) {
    if (isTreeCreated) {
        delete tree;
        isTreeCreated = 0;
        return tree;
    }
}

Tree* CreateRandomTree(Tree* tree) {
    int key;
    int count;
    cout << "Enter the number of elements";
    cin >> count;
    if (count > 0) {
        for (int i = 0; i < count; i++) {
            key = rand() % 100;
            tree = InsertElement(key, tree, 1);
        }
    }
    return tree;
}

Tree* CreateNoRandomTree(Tree* tree) {

    int key;
    while (true) {
        cout << "Enter number: ";
        if (cin >> key) {
            cout << key << " ";
            tree = InsertElement(key, tree, 1);
        }
        else {
            cin.clear();
            cin.ignore();
            break;
        }

    }
}

Tree* CreateFromFileTree(Tree* tree) {
    int key;
    ifstream File(input_tree);
    while (!File.eof()) {
        File >> key;
        tree = InsertElement(key, tree, 1);
    }
    File.close();
    return tree;
}

void PrintTreeRecursive(Tree* tree, ostream& output) {
    if (tree == nullptr)
        return;

    PrintTreeRecursive(tree->left, output);
    SetConColor(tree->color, WHITE);
    output << tree->value << " ";
    PrintTreeRecursive(tree->right, output);
}

void PrintTree(Tree* tree, bool isConsole) {
    if (isConsole) {
        PrintTreeRecursive(tree, cout);
        cout << endl;
    }
    else {
        ofstream file(output_tree);
        if (file.is_open()) {
            PrintTreeRecursive(tree, file);
            file.close();
        }
        else {
            cout << "Unable to open file" << endl;
        }
    }
}


void PreOrderTravers(Tree* tree) {
    if (tree) {
        cout << tree->value << " ";
        PreOrderTravers(tree->left);
        PreOrderTravers(tree->right);
    }
}

void PostOrderTravers(Tree* tree) {
    if (tree) {
        PostOrderTravers(tree->left);
        PostOrderTravers(tree->right);
        cout << tree->value << " ";
    }
}

void BreadthFirstSearch(Tree* tree) {
    queue<Tree*> nodeQueue;
    Tree* root = new Tree;
    root = tree;
    nodeQueue.push(root);
    while (!nodeQueue.empty()) {
        root = nodeQueue.front();
        cout << root->value << " ";
        nodeQueue.pop();
        if (root->left) {
            nodeQueue.push(root->left);
        }
        if (root->right) {
            nodeQueue.push(root->right);
        }
    }
}

Tree* GenerationTasks(Tree* tree, int countOfTasks) {
    ofstream Output_Task;
    Output_Task.open(output_task);
    Output_Task.clear();
    Output_Task.close();
    ofstream Output_Key;
    Output_Key.open(output_key);
    Output_Key.clear();
    Output_Key.close();
    ofstream Output_Ans;
    Output_Ans.open(output_ans);
    Output_Ans.clear();
    Output_Ans.close();
    int task;
    int key;
    DestroyTree(tree);
    tree = new Tree;
    CreateRandomTree(tree);
    PrintTree(tree, 0);
    Output_Ans.open(output_ans, ios::app | ios::ate);
    Output_Ans << "\n\n\n";
    Output_Ans.close();
    for (int i = 0; i < countOfTasks; i++) {
        task = rand() % 2;
        switch (task) {
        case 0: {
            key = rand() % 99;
            Output_Task.open(output_task, ios::app);
            Output_Task << "Задание: удалить число " << key << '\n';
            Output_Task.close();
            DeleteElement(key, tree, 0);
            PrintTree(tree, 0);
            Output_Ans.open(output_ans, ios::app | ios::ate);
            Output_Ans << i << "\n";
            Output_Ans.close();
            break;
        }
        case 1: {
            key = rand() % 99;
            Output_Task.open(output_task, ios::app);
            Output_Task << "Задание: вставить число " << key << '\n';
            Output_Task.close();
            InsertElement(key, tree, 0);
            PrintTree(tree, 0);
            Output_Ans.open(output_ans, ios::app | ios::ate);
            Output_Ans << i << "\n";
            Output_Ans.close();
            break;
        }
        }
    }
    return tree;
}

int main() {
    Tree* tree = new Tree;
    unsigned short number_of_task;
    tree->value = 0;
    tree->left = 0;
    tree->right = 0;
    int key;
    while (true) {
        SetConColor(WHITE, BLACK);
        cout << "Choose the task number to which you want to gain access:\n"
            "\n"
            "1.Creating random elements.\n"
            "2.Creating specified elements.\n"
            "3.Creating numbers from a file.\n"
            "4.Printing to the console.\n"
            "5.Printing to a file.\n"
            "6.Inserting an element and measuring the insertion time.\n"
            "7.Deleting an element and measuring the deletion time.\n"
            "8.Retrieving an element and measuring the retrieval time.\n"
            "9.Preorder traversal.\n"
            "10.Postorder traversal.\n"
            "11.Breadth-first traversal.\n"
            "12.Generate task(s)."
            "To exit, enter 0.\n"
            "Number:";
        cin >> number_of_task;
        switch (number_of_task) {
        case 0: {
            cout << "\nExiting...\n";
            DestroyTree(tree);
            return 0;
        }
        case 1: {
            tree = DestroyTree(tree);

            tree = CreateRandomTree(tree);
            break;
        }
        case 2: {
            tree = DestroyTree(tree);

            tree = CreateNoRandomTree(tree);
            break;
        }
        case 3: {
            tree = DestroyTree(tree);

            tree = CreateFromFileTree(tree);
            break;
        }
        case 4: {
            PrintTree(tree, 1);
            break;
        }
        case 5: {
            ofstream File;
            File.open(output_tree);
            File.clear();
            File.close();
            PrintTree(tree, 0);
            break;
        }
        case 6: {
            cout << "Enter number which you want to add: ";
            cin >> key;
            unsigned int start_time = clock();
            InsertElement(key, tree, 1);
            unsigned int end_time = clock();
            unsigned int search_time = end_time - start_time;
            cout << "Num added.Lead time is  " << search_time << " ms\n";
            break;
        }
        case 7: {
            cout << "Enter number which you want to delete: ";
            cin >> key;
            unsigned int start_time = clock();
            tree = DeleteElement(key, tree, 1);
            unsigned int end_time = clock();
            unsigned int search_time = end_time - start_time;
            cout << "Lead time: " << search_time << " ms\n";
            break;
        }
        case 8: {
            cout << "Enter number which you want to find in tree: ";
            cin >> key;
            unsigned int start_time = clock();
            SearchElement(key, tree);
            unsigned int end_time = clock();
            unsigned int search_time = end_time - start_time;
            cout << "Lead time: " << search_time << " ms\n";
            break;
        }
        case 9: {
            PreOrderTravers(tree);
            cout << "\n";
            break;
        }
        case 10: {
            PostOrderTravers(tree);
            cout << "\n";
            break;
        }
        case 11: {
            BreadthFirstSearch(tree);
            cout << "\n";
            break;
        }
        case 12: {
            int countOfTasks;
            cout << "How many task you want to generate?: ";
            cin >> countOfTasks;
            tree = GenerationTasks(tree, countOfTasks);
            break;
        }
        default: {
            cout << "\nERROR: Task not found\n";
            break;
        }
        }
    }
}
