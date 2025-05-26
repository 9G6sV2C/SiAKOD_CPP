#pragma once

#include <iostream>
#include <string>

using namespace std;


template <typename TKey, typename TVal> class AVLTree;
template <typename TKey, typename TVal> class BinarySearchTree;

template <typename TKey, typename TVal> class TreeNode {
    template <typename TKey, typename TVal> friend class AVLTree;

public:
    TreeNode(TKey key, TVal value) : _key(key), _value(value), _left(nullptr), _right(nullptr), _height(0) { }
    TreeNode(const TreeNode&) = delete;

    ~TreeNode() { }

    // Возвращаем ссылку на поле класса, без копировния, + это lvalue
    const TKey& getKey() const { return _key; }
    // Возвращаем ссылку на поле класса, без копировния, + это lvalue
    const TVal& getValue() const { return _value; }

    // Возвращаем ссылку на поле класса, без копировния, + это lvalue
    // Нарушает инкапсуляцию
    TVal& getValue() { return _value; }

    friend ostream& operator<<(ostream& os, const TreeNode& treeNode) {
        os << "{ " << treeNode.getKey() << " } --> { " << treeNode.getValue() << " } ";
        os << "(h:" << treeNode._height << ")";
        return os;
    }

private:
    TKey _key;
    TVal _value;
    TreeNode* _left;
    TreeNode* _right;
    int _height;
};

template <typename TKey, typename TVal> class AVLTree {
public:
    AVLTree() : _root(nullptr), _size(0) { }
    //AVLTree(const AVLTree&) = delete;

    ~AVLTree() { deleteAllHelperR(_root); }

    // Если такой узел уже есть, то ничего не произойдёт
    void insert(TKey key, TVal value) { _root = insertHelperR(_root, key, value); ++_size; }

    // Удаляет эл., если такого нет, то ничего не произойдёт
    void del(TKey key) { _root = deleteHelperR(_root, key); --_size; }

    TreeNode<TKey, TVal>* find(TKey key) const { return findHelper(_root, key); }

    void print() const {
        cout << "\n\\/ \\/ \\/ \\/ \\/ \\/ \\/ \\/ \\/ \n\n";

        printHelperR(_root); cout << '\n';

        cout << "/\\ /\\ /\\ /\\ /\\ /\\ /\\ /\\ /\\ \n\n";
    }

    int size() const { return _size; }

    TreeNode<TKey, TVal>* getRoot() { return _root; }

private:
    TreeNode<TKey, TVal>* _root;
    int _size;

    int height(TreeNode<TKey, TVal>* node) { return (node != nullptr) ? node->_height : 0; }

#pragma region Балансировка

    int balanceFactor(TreeNode<TKey, TVal>*& node) {
        return (node != nullptr) ? (height(node->_left) - height(node->_right)) : 0;
    }

    void updateHeight(TreeNode<TKey, TVal>*& node) {
        if (node != nullptr) {
            node->_height = 1 + max(height(node->_left), height(node->_right));
        }
    }

    // Балансировка дерева
    TreeNode<TKey, TVal>* rotateRight(TreeNode<TKey, TVal>* y) {
        TreeNode<TKey, TVal>* x = y->_left;
        TreeNode<TKey, TVal>* T2 = x->_right;

        // Выполняем поворот
        x->_right = y;
        y->_left = T2;

        // Обновляем высоты
        updateHeight(y);
        updateHeight(x);

        return x;
    }

    TreeNode<TKey, TVal>* rotateLeft(TreeNode<TKey, TVal>* x) {
        TreeNode<TKey, TVal>* y = x->_right;
        TreeNode<TKey, TVal>* T2 = y->_left;

        // Выполняем поворот
        y->_left = x;
        x->_right = T2;

        // Обновляем высоты
        updateHeight(x);
        updateHeight(y);

        return y;
    }

    // Проверяет баланс дерева и выполняет необходимые повороты
    TreeNode<TKey, TVal>* balance(TreeNode<TKey, TVal>* node) {
        if (!node) return nullptr;

        updateHeight(node);
        int bf = balanceFactor(node);

        // Left Left
        // включение в левое поддерево левого поддерева
        /*
            A (bal = 2)
           /
          B (bal >= 0)
         /
        C

        */
        if (bf > 1 && balanceFactor(node->_left) >= 0) {
            return rotateRight(node);
        }

        // Left Right
        // включение в правое поддерево левого поддерева
        /*
            A (bal = 2)
           /
          B (bal = -1)
           \
           C

        */
        if (bf > 1 && balanceFactor(node->_left) < 0) {
            node->_left = rotateLeft(node->_left);
            return rotateRight(node);
        }

        // Right Right
        // включение в правое поддерево правого поддерева
         /*
            A (bal = -2)
             \
              B (bal <= 0)
               \
                C

        */
        if (bf < -1 && balanceFactor(node->_right) <= 0) {
            return rotateLeft(node);
        }

        // Right Left 
        // включение в правое поддерево левого поддерева
        /*
            A (bal = -2)
             \
              B (bal = 1)
             /
            C

        */
        if (bf < -1 && balanceFactor(node->_right) > 0) {
            node->_right = rotateRight(node->_right);
            return rotateLeft(node);
        }

        return node; // Дерево сбалансировано
    }

#pragma endregion

    TreeNode<TKey, TVal>* insertHelperR(TreeNode<TKey, TVal>*& currRoot, TKey& key, TVal& value) {
        if (currRoot == nullptr) {
            return new TreeNode<TKey, TVal>(key, value);
        }

        if (key < currRoot->_key) {
            currRoot->_left = insertHelperR(currRoot->_left, key, value);
        }
        else if (key > currRoot->_key) {
            currRoot->_right = insertHelperR(currRoot->_right, key, value);
        }
        else {
            // Такой ключ уже сущ., уменьшаем счётчик т.к. в public методе он в любом случае увеличится 
            --_size;
            return currRoot;
        }

        return balance(currRoot);
    }

    TreeNode<TKey, TVal>* deleteHelperR(TreeNode<TKey, TVal>*& currRoot, TKey& key) {
        if (currRoot == nullptr) {
            ++_size; // Компенсируем уменьшение в public методе
            return nullptr;
        }

        // else-ы для красоты
        // Спускаемся до удаляемого узла
        if (key < currRoot->_key) {
            currRoot->_left = deleteHelperR(currRoot->_left, key);
            return currRoot;
        }
        else if (key > currRoot->_key) {
            currRoot->_right = deleteHelperR(currRoot->_right, key);
            return currRoot;
        }

        else {
            // Узел с одним или без детей
            if (currRoot->_left == nullptr || currRoot->_right == nullptr) {
                TreeNode<TKey, TVal>* temp = (currRoot->_left != nullptr) ? currRoot->_left : currRoot->_right;

                // Нет детей
                if (temp == nullptr) {
                    temp = currRoot;
                    currRoot = nullptr;
                }
                else { // Один ребенок
                    *currRoot = *temp; // Копируем содержимое
                }

                delete temp;
            }
            else {
                // Узел с двумя детьми
                TreeNode<TKey, TVal>* temp = getLeftSuccessor(currRoot);
                currRoot->_key = temp->_key;
                currRoot->_value = temp->_value;
                currRoot->_right = deleteHelperR(currRoot->_right, temp->_key);
            }
        }

        if (currRoot == nullptr) {
            return nullptr;
        }

        return balance(currRoot);
    }

    // !!! Передаём не по ссылке т.к. будем изменять
    // Возвращает указатель на найденный узел, если такого нет, то return nullptr
    // Это лучше чем return bool, т.к. даёт больше информации
    TreeNode<TKey, TVal>* findHelper(TreeNode<TKey, TVal>* currRoot, TKey& key) const {
        while (currRoot != nullptr) {
            if (key < currRoot->_key) {
                currRoot = currRoot->_left;
            }
            else if (key > currRoot->_key) {
                currRoot = currRoot->_right;
            }
            else { // =
                break;
            }
        }

        return currRoot;
    }

    // !!! Передаём не по ссылке т.к. будем изменять
    // Поиск наименьшего (самого левого) в ПРАВОМ поддереве
    TreeNode<TKey, TVal>* getLeftSuccessor(TreeNode<TKey, TVal>* currRoot) {
        currRoot = currRoot->_right;

        while (currRoot != nullptr && currRoot->_left != nullptr) {
            currRoot = currRoot->_left;
        }

        return currRoot;
    }

    // Концевой обход
    void deleteAllHelperR(TreeNode<TKey, TVal>*& currRoot) {
        if (currRoot != nullptr) {
            deleteAllHelperR(currRoot->_left);
            deleteAllHelperR(currRoot->_right);
            //cout << "------ " << *currRoot << " is deleted\n";
            delete currRoot;
        }
    }

    void printHelperR(const TreeNode<TKey, TVal>* const& currRoot) const {
        if (currRoot != nullptr) {
            printHelperR(currRoot->_left);
            cout << *currRoot << '\n';
            printHelperR(currRoot->_right);
        }
    }
};

template <typename TKey, typename TVal> class BinarySearchTree {
public:
    BinarySearchTree() : _root(nullptr), _size(0) { }
    //BinarySearchTree(const BinarySearchTree&) = delete;

    ~BinarySearchTree() { deleteAllHelperR(_root); }

    // Если такой узел уже есть, то ничего не произойдёт
    void insert(TKey key, TVal value) { _root = insertHelperR(_root, key, value); ++(this->_size); }

    // Удаляет эл., если такого нет, то ничего не произойдёт
    void del(TKey key) { _root = deleteHelperR(_root, key); --(this->_size); }

    TreeNode<TKey, TVal>* find(TKey key) const { return findHelper(_root, key); }

    void print() const {
        cout << "\n\\/ \\/ \\/ \\/ \\/ \\/ \\/ \\/ \\/ \n\n";
        printHelperR(_root); cout << '\n';
        cout << "/\\ /\\ /\\ /\\ /\\ /\\ /\\ /\\ /\\ \n\n";
    }

    int size() const { return _size; }

    TreeNode<TKey, TVal>* getRoot() { return _root; }

private:
    TreeNode<TKey, TVal>* _root;
    int _size;

    TreeNode<TKey, TVal>* insertHelperR(TreeNode<TKey, TVal>*& currRoot, TKey& key, TVal& value) {
        if (currRoot == nullptr) {
            return new TreeNode<TKey, TVal>(key, value);
        }

        // Можно ли обойтись без присваиваний?
        if (key < currRoot->_key) {
            currRoot->_left = insertHelperR(currRoot->_left, key, value);
        }
        else if (key > currRoot->_key) {
            currRoot->_right = insertHelperR(currRoot->_right, key, value);
        }

        return currRoot;
    }

    TreeNode<TKey, TVal>* deleteHelperR(TreeNode<TKey, TVal>*& currRoot, TKey& key) {
        if (currRoot == nullptr) {
            return nullptr;
        }

        // else-ы для красоты
        // Спускаемся до удалякмого узла
        if (key < currRoot->_key) {
            currRoot->_left = deleteHelperR(currRoot->_left, key);
            return currRoot;
        }
        else if (key > currRoot->_key) {
            currRoot->_right = deleteHelperR(currRoot->_right, key);
            return currRoot;
        }

        else {
            // 1. Нет дочерних
            if (currRoot->_left == nullptr && currRoot->_right == nullptr) {
                delete currRoot;
                return nullptr;
            }

            // 2. Один дочений
            else if (currRoot->_left == nullptr) {
                TreeNode<TKey, TVal>* temp = currRoot->_right;
                delete currRoot;
                return temp;
            }
            else if (currRoot->_right == nullptr) {
                TreeNode<TKey, TVal>* temp = currRoot->_left;
                delete currRoot;
                return temp;
            }

            // 3. Два дочерних
            else {
                TreeNode<TKey, TVal>* succ = getLeftSuccessor(currRoot); // находим 'наследника' (тот на который будем заменять)
                currRoot->_key = succ->_key; // Заменяем ключи
                currRoot->_right = deleteHelperR(currRoot->_right, succ->_key);
                return currRoot;
            }
        }
    }

    // !!! Передаём не по ссылке т.к. будем изменять
    // Возвращает указатель на найденный узел, если такого нет, то return nullptr
    // Это лучше чем return bool, т.к. деёт больше информации
    TreeNode<TKey, TVal>* findHelper(TreeNode<TKey, TVal>* currRoot, TKey& key) const {
        while (currRoot != nullptr) {
            if (key < currRoot->_key) {
                currRoot = currRoot->_left;
            }
            else if (key > currRoot->_key) {
                currRoot = currRoot->_right;
            }
            else { // =
                break;
            }
        }

        return currRoot;
    }

    // !!! Передаём не по ссылке т.к. будем изменять
    // Поиск наименьшего (самого левого) в ПРАВОМ поддереве
    TreeNode<TKey, TVal>* getLeftSuccessor(TreeNode<TKey, TVal>* currRoot) {
        currRoot = currRoot->_right;

        while (currRoot != nullptr && currRoot->_left != nullptr) {
            currRoot = currRoot->_left;
        }

        return currRoot;
    }

    // Концевой обход
    void deleteAllHelperR(TreeNode<TKey, TVal>*& currRoot) {
        if (currRoot != nullptr) {
            deleteAllHelperR(currRoot->_left);
            deleteAllHelperR(currRoot->_right);
            //cout << "------ " << *currRoot << " is deleted\n";
            delete currRoot;
        }
    }

    void printHelperR(const TreeNode<TKey, TVal>* const& currRoot) const {
        if (currRoot != nullptr) {
            printHelperR(currRoot->_left);
            cout << *currRoot << '\n';
            printHelperR(currRoot->_right);
        }
    }
};