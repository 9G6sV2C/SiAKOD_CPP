#include <iostream>

using namespace std;

template <typename T>
class Node {
public:
    Node(T data, Node* next = nullptr) : _data(data), _nextPtr(next) { }
    ~Node() {}
    //private:
    T _data;
    Node* _nextPtr;
};

template <typename T>
class MyQueue {
public:
    MyQueue() : _headPtr(nullptr), _tailPtr(nullptr), _size(0) { }

    ~MyQueue() {
        while (!isEmpty()) {
            pop();
        }
    }

    bool isEmpty() { return _size == 0; }

    size_t getSize() { return _size; }

    // Добавление в конец
    void push(int data) {
        // Создание новой Node в куче
        Node<T>* ptrNewNode = new Node<T>(data);
        if (isEmpty()) {
            _headPtr = _tailPtr = ptrNewNode;
        }
        else {
            // Хвост смотрит на новую ячейку
            _tailPtr->_nextPtr = ptrNewNode;
            // Теперь старый хвост не хвост, и хвостом является новая ячейка
            // (Обновляем указатель на хвост)
            _tailPtr = ptrNewNode;
        }
        ++_size;
    }

    // Удаление head
    void pop() {
        if (isEmpty()) {
            throw runtime_error("Can't pop, queue is empty.");
        }
        // Запоминаем указатель на голову
        Node<T>* ptrNodeToDelete = _headPtr;

        // Сдвигаем
        _headPtr = _headPtr->_nextPtr;

        // Если после этого очередь опустела, обновляем хвост
        if (_headPtr == nullptr) {
            _tailPtr = nullptr;
        }

        delete ptrNodeToDelete;
        --_size;
        //return data;
    }

    // Получение head без удаления
    int get() {
        if (isEmpty()) {
            throw runtime_error("Can't get, queue is empty.");
        }
        return _headPtr->_data;
    }

    // Вывод всей очереди ОТ head ДО tail
    void printQ() {
        Node<T>* currPtrNode = _headPtr;
        while (currPtrNode != nullptr) {
            cout << currPtrNode->_data;

            currPtrNode = currPtrNode->_nextPtr;

            if (currPtrNode != nullptr) {
                cout << " --> ";
            }
        }
        cout << '\n';
    }

    //void DecHead() { --(_headPtr->_data); }
private:
    Node<T>* _headPtr;
    Node<T>* _tailPtr;
    size_t _size;
};

//class CashBox {
//public:
//    CashBox() : _queue(MyQueue()), _sum(0) { }
//
//    ~CashBox() { _queue.~MyQueue(); }
//
//    int getSum() { return _sum; }
//
//    bool isEmpty() { return _queue.isEmpty(); }
//
//    size_t getSize() { return _queue.getSize(); }
//
//    void push(int data) { _queue.push(data); _sum += data; }
//
//    int pop() {
//        int temp = _queue.pop();
//        _sum -= temp;
//        return temp;
//    }
//
//    int get() { return _queue.get(); }
//
//    void printQ() { _queue.printQ(); }
//
//    void DecHead() { _queue.DecHead(); --_sum; }
//private:
//    MyQueue _queue;
//    int _sum;
//};

int main() {
    string s = "EAS*Y*QUE***ST***IO*N**";
    string res = "";
    MyQueue<char> queue;

    for (char ch : s) {
        if (ch != '*') {
            queue.push(ch);
        }
        else {
            res += queue.get();
            queue.pop();
        }
    }

    cout << res;

    return 0;
}