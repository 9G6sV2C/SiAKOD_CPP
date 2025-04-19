#include <iostream>

using namespace std;

template <typename T>
class MyClass {
private:
    T value;

    // Вспомогательная функция для вывода
    void printHelper(const T& val) const {
        std::cout << "Generic: " << val << std::endl;
    }


    /*Перегрузка для std::pair*/
    template <typename U, typename V>
    void printHelper(const std::pair<U, V>& val) const {
        std::cout << "Pair: (" << val.first << ", " << val.second << ")" << std::endl;
    }


    void printHelper(const pair<int, int>& val) const {
        cout << "Pair: (" << val.first << ", " << val.second << ")" << std::endl;
    }

public:
    MyClass(const T& val) : value(val) { }

    void print() const {
        printHelper(value);
    }
};

int main() {
    for (int _ = 0; _ < 5; ++_) {
        auto f = false;
        cout << f;
    }
    return 0;
}