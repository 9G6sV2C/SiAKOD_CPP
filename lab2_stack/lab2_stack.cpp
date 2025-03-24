#include <iostream>
#include <vector>
#include <stack>

using namespace std;

class NodeInt {
public:
    NodeInt(int data, NodeInt* next = nullptr) : _data(data), _nextPtr(next) { }
    ~NodeInt() {}
    //private:
    int _data;
    NodeInt* _nextPtr;
};

class Stack {
public:
    Stack() : _topPtr(nullptr), _size(0) { }

    ~Stack() {
        while (!isEmpty()) {
            pop();
        }
    }

    bool isEmpty() { return _size == 0; }

    size_t getSize() { return _size; }

    // Добавление
    void push(int data) {
        NodeInt* newNodePtr = new NodeInt(data);

        // Новый узел указывает на текущий верхний узел
        newNodePtr->_nextPtr = _topPtr;

        // Обновляем указатель на вершину
        _topPtr = newNodePtr;

        ++_size;
    }

    // Удаление top
    void pop() {
        if (isEmpty()) {
            throw runtime_error("Can't pop, queue is empty.");
        }

        // Запоминаем указатель на голову
        NodeInt* nodeToDelete = _topPtr;

        // Сдвигаем
        _topPtr = _topPtr->_nextPtr;

        delete nodeToDelete;
        --_size;
    }

    // Получение top
    int get() {
        if (isEmpty()) {
            throw runtime_error("Can't get, queue is empty.");
        }
        return _topPtr->_data;
    }

    void printS() {
        NodeInt* currNodePtr = _topPtr;

        cout << "{ ";
        while (currNodePtr != nullptr) {
            cout << currNodePtr->_data;
            currNodePtr = currNodePtr->_nextPtr;
            if (currNodePtr != nullptr) {
                cout << " --> ";
            }
        }
        cout << " }";
    }
private:
    NodeInt* _topPtr;
    size_t _size;
};

void replaceWithNextGreater(vector<int>& A) {
    size_t n = A.size();
    stack <int> st;

    // Проходим массив с конца к началу
    for (size_t i = A.size(); i > 0; i--) {

        // Удаляем элементы из стека, которые меньше или равны текущему элементу
        if (st.size() > 0) {
            while (st.size() > 0 && (st.top() <= A[i - 1])) {
                st.pop();
            }
        }

        // Если стек не пуст, верхний элемент — это ближайший больший элемент
        // Если стек пуст, заменяем на 0
        int t = A[i - 1];
        A[i - 1] = st.size() > 0 ? st.top() : 0;

        // Добавляем текущий элемент в стек
        st.push(t);
    }
    for (size_t i = 0; i < A.size(); i++)
        cout << A[i] << ' ';
    cout << '\n';
}

int main() {
    vector<int> A = {1, 3, 2, 5, 3, 4};

    replaceWithNextGreater(A);

    for (int num : A) {
        cout << num << " ";
    }
    cout << endl;

    Stack stack;
    vector<int> input_arr = {1, 3, 2, 5, 3, 4};

    for (int el : input_arr) {
        stack.push(el);
    }

    stack.printS(); // { 4 --> 3 --> 5 --> 2 --> 3 --> 1 }

    return 0;
}