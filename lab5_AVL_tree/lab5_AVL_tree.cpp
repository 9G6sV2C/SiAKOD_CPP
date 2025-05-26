#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <chrono>
#include "lab5_AVL_and_BST_trees.h"

using namespace std;

#pragma region Автобусы

enum bus_status { on_route, in_park, NaN };

istream& operator>>(istream& ins, bus_status& bss) {
    string s;
    ins >> s;

    if (s == "on_route") bss = bus_status::on_route;
    else if (s == "in_park") bss = bus_status::in_park;
    else bss = bus_status::NaN;

    return ins;
}

ostream& operator<<(ostream& os, const bus_status& bss) {
    switch (bss) {
        case bus_status::in_park: { os << "in_park"; break; }
        case bus_status::on_route: { os << "on_route"; break; }
        default: { os << "NaN"; break; }
    }
    return os;
}

class Bus {
    friend class BusDB;

    string _driver_name = "???";
    int _bus_route = -1;
    bus_status _status = bus_status::NaN;;
public:
    Bus() { }
    Bus(string driverName, int busRoute, bus_status status) : _driver_name(driverName), _bus_route(busRoute), _status(status) { }
    Bus(string driverName, int busRoute) : _driver_name(driverName), _bus_route(busRoute), _status(bus_status::NaN) { }
    ~Bus() { }

    friend istream& operator>>(istream& ins, Bus& bs) {
        string driverName;
        int busRoute;
        bus_status status;

        ins >> driverName >> busRoute >> status;

        bs._driver_name = driverName;
        bs._bus_route = busRoute;
        bs._status = status;

        return ins;
    }

    friend ostream& operator<<(ostream& os, const Bus& bs) {
        os << bs._driver_name << " ~~ " << bs._bus_route << " ~~ " << bs._status;
        return os;
    }

    bool operator==(const Bus& other) const {
        return (this->_driver_name == other._driver_name &&
                this->_bus_route == other._bus_route &&
                this->_status == other._status);
    }
};

class BusDB {
public:
    BusDB() { }

    ~BusDB() { }

    // Возможно потом сделаю через initializer_list
    BusDB(int n) {
        int currBusNum;
        Bus currBus;

        for (int _ = 0; _ < n; ++_) {
            cin >> currBusNum >> currBus;

            this->insertBus(currBusNum, currBus);
        }
    }

    void insertBus(int busNum, Bus bus) { _busDB.insert(busNum, bus); }

    void delBus(int busNum) { _busDB.del(busNum); }

    TreeNode<int, Bus>* findBus(int busNum) const { return _busDB.find(busNum); }

    void toGo(int busNum) { findBus(busNum)->getValue()._status = bus_status::on_route; }
    void toRest(int busNum) { findBus(busNum)->getValue()._status = bus_status::in_park; }

    void printBusDB() const { _busDB.print(); }

    int size() { return _busDB.size(); }

    TreeNode<int, Bus>* getBusRoot() { return _busDB.getRoot(); }

private:
    // номерАвтобуса_классАвтобус
    AVLTree<int, Bus> _busDB;
};

#pragma endregion

inline int rnd(int A, int B) { return A + rand() % (B - A + 1); }

#pragma region Демонстрация работы АВЛ-дерева

void demoBasicOperations() {
    cout << "=== demoBasicOperations ===\n";

    AVLTree<int, string> tree;

    tree.insert(50, "Fifty");
    tree.insert(30, "Thirty");
    tree.insert(70, "Seventy");
    tree.insert(20, "Twenty");
    tree.insert(40, "Forty");
    tree.insert(60, "Sixty");
    tree.insert(80, "Eighty");

    /*
        50
      /    \
    30      70
   /  \    /  \
20    40 60    80

    */
    cout << tree.size() << '\n';
    tree.print();

    // Поиск элементов
    int keyToFind = 40;
    cout << "keyToFind = " << keyToFind << '\n';
    auto* node = tree.find(keyToFind);
    if (node) {
        cout << *node << '\n';
    }
    else {
        cout << "Not found.\n";
    }

    keyToFind = 45;
    cout << "keyToFind = " << keyToFind << '\n';
    node = tree.find(keyToFind);
    if (node) {
        cout << *node << '\n';
    }
    else {
        cout << "Not found.\n";
    }

    cout << "Deleting keys 20 and 70...\n";
    tree.del(20);
    tree.del(70);
    /*

        50
      /    \
    30      60
      \      \
      40      80

    */
    cout << tree.size() << '\n';
    tree.print();
}

void demoBalance() {
    cout << "\n=== demoBalance ===\n";

    AVLTree<int, string> tree;

    // Вставка элементов, которые могут вызвать дисбаланс
    tree.insert(10, "ten");
    tree.insert(20, "twenty");
    tree.insert(30, "thirty");
    tree.insert(40, "forty");
    tree.insert(50, "fifty");
    tree.insert(25, "twenty five");

    /*
        25
      /    \
    15      40
   /  \    /  \
10    20 30    50

    */
    cout << tree.size() << '\n';
    tree.print();

    cout << "Deleting nodes with keys 40 and 50...\n";
    // Удаление, которое может вызвать дисбаланс
    tree.del(40);
    tree.del(50);

    /*
        25
      /    \
    15      30
   /
10
    */
    cout << tree.size() << '\n';
    tree.print();
}

void demoUpdateValue() {
    cout << "\n=== demoUpdateValue ===\n";

    AVLTree<int, string> tree;

    tree.insert(1, "One");
    tree.insert(2, "Two");
    tree.insert(3, "Three");

    cout << tree.size() << '\n';
    tree.print();

    cout << "Updating key 2...\n";
    auto node = tree.find(2);
    if (node) {
        node->getValue() = "Two (updated)";
    }

    cout << tree.size() << '\n';
    tree.print();
}

void demo_AVL() {
    cout << "--------------------- demo_AVL -------------------------------\n\n";
    demoBasicOperations();
    demoBalance();
    demoUpdateValue();
    cout << "--------------------------------------------------------------\n\n";
}

#pragma endregion

#pragma region Измерения времени выполнения

chrono::microseconds measureLinearSearch(vector<pair<int, Bus>>& arrBuses, pair<int, Bus>& toFind) {
    auto start_i = chrono::high_resolution_clock::now();
    for (const auto& curr : arrBuses) {
        if (curr == toFind) {
            break;
        }
    }
    auto stop_i = chrono::high_resolution_clock::now();

    return chrono::duration_cast<chrono::microseconds>(stop_i - start_i);
}

chrono::microseconds measureAVLTreeSearch(BusDB& bdb, pair<int, Bus>& toFind) {
    auto start_i = chrono::high_resolution_clock::now();
    bdb.findBus(toFind.first);
    auto stop_i = chrono::high_resolution_clock::now();

    return chrono::duration_cast<chrono::microseconds>(stop_i - start_i);
}

chrono::microseconds measureBinSearch(vector<pair<int, Bus>> arrBuses_sorted, pair<int, Bus>& toFind) {
    int l_i = 0;
    int r_i = arrBuses_sorted.size() - 1;
    int m_i = (l_i + r_i) / 2;
    bool flag = false;

    auto start_i = chrono::high_resolution_clock::now();
    for (; (l_i <= r_i) && !flag; ) {
        m_i = (l_i + r_i) / 2;

        if (arrBuses_sorted[m_i].first < toFind.first) {
            l_i = m_i + 1;
        }
        else if (arrBuses_sorted[m_i].first > toFind.first) {
            r_i = m_i - 1;
        }
        else {
            flag = true;
        }
    }
    auto stop_i = chrono::high_resolution_clock::now();

    return chrono::duration_cast<chrono::microseconds>(stop_i - start_i);
}

#pragma endregion

int main() {
    srand(time(0));

    demo_AVL();
    
    BusDB bdb;
    vector<pair<int, Bus>> arrBuses;

    // '' случайно сгенерированных автобусов добовляем в arrBuses
    for (int i = 0; i < 200000; i += rnd(1, 3)) {
        // "красивые" символы ASCII
        string currName(1, static_cast<char>(rnd(33, 126)));

        int currRoute = rnd(1, 100);

        bus_status currStatus;
        int tempR = rnd(0, 2);
        switch (tempR) {
            case 0: currStatus = bus_status::in_park; break;
            case 1: currStatus = bus_status::on_route; break;
            default:currStatus = bus_status::NaN; break;
        }

        Bus currBus(currName, currRoute, currStatus);

        arrBuses.push_back(pair<int, Bus>(i, currBus));
    }

    // Копия arrBuses, отсортированого по номерам, пригодиться для бин. поиска
    vector<pair<int, Bus>> arrBuses_sorted = arrBuses;

    // Перемешиваем массив
    random_shuffle(arrBuses.begin(), arrBuses.end());

    // Перемешанный массив поэлеметно добавляем в BST
    for (int i = 0; i < arrBuses.size(); ++i) {
        bdb.insertBus(arrBuses[i].first, arrBuses[i].second);
    }

#pragma region Вывод автобусов
    //for (int i = 1; i < arrBuses.size(); ++i) {
    //    cout << arrBuses[i].first << " -- " << arrBuses[i].second << '\n';
    //}

    //cout << "*(bdb.getBusRoot()) = " << *(bdb.getBusRoot()) << '\n' << '\n';

    //bdb.printBusDB();

    //cout << "--------------------------------------------------------------\n\n";
#pragma endregion

    pair<int, Bus> toFind;
    auto start_i = chrono::high_resolution_clock::now();
    auto stop_i = chrono::high_resolution_clock::now();
    auto duration_total_linear = chrono::duration_cast<chrono::microseconds>(stop_i - start_i);
    auto duration_total_avl = chrono::duration_cast<chrono::microseconds>(stop_i - start_i);
    auto duration_total_binSearch = chrono::duration_cast<chrono::microseconds>(stop_i - start_i);

    // Для усложнения, ищем '' случайных элементов
    for (int _ = 0; _ < 100; ++_) {
        toFind = arrBuses[rnd(0, arrBuses.size())];

        //cout << _+1 << ") ";

        // Линейный поиск в массиве
        duration_total_linear += measureLinearSearch(arrBuses, toFind);

        // Поиск в дереве
        duration_total_avl += measureAVLTreeSearch(bdb, toFind);


        // Бинарный поиск в массиве arrBuses_sorted
        duration_total_binSearch += measureBinSearch(arrBuses_sorted, toFind);
    }

    cout << '\n';

    cout << duration_total_linear.count() << ' ' << duration_total_avl.count() << ' ' 
         << duration_total_binSearch.count() << '\n';

    return 0;
}