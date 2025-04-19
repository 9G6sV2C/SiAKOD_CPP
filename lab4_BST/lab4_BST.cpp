#include <iostream>
#include <string>
#include <initializer_list>
#include <vector>
#include <random>
#include <chrono>

using namespace std;


template <typename TKey, typename TVal> class BinarySearchTree;

template <typename TKey, typename TVal> class TreeNode {
    template <typename TKey, typename TVal> friend class BinarySearchTree;

public:
    TreeNode(TKey key, TVal value) : _key(key), _value(value), _left(nullptr), _right(nullptr) { }
    TreeNode(const TreeNode&) = delete;

    ~TreeNode() { }

    // Возвращаем ссылку на поле класса, без копировния, + это lvalue
    const TKey& getKey() const { return _key; }
    // Возвращаем ссылку на поле класса, без копировния, + это lvalue
    const TVal& getValue() const { return _value; }

    // Возвращаем ссылку на поле класса, без копировния, + это lvalue
    // Можно нарушать инкапсуляцию
    TVal& getValue() { return _value; }

    //void setValue(TVal value) { _value = value; }

    friend ostream& operator<<(ostream& os, const TreeNode& treeNode) {
        os << "{ " << treeNode.getKey() << " } --> { " << treeNode.getValue() << " }";
        return os;
    }
private:
    TKey _key;
    TVal _value;
    TreeNode* _left;
    TreeNode* _right;
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

    void printHelperR(const TreeNode<TKey, TVal>*const& currRoot) const {
        if (currRoot != nullptr) {
            printHelperR(currRoot->_left);
            cout << *currRoot << '\n';
            printHelperR(currRoot->_right);
        }
    }

    // Не используется.
    // Спецификация если _value -- pair
    /*
    template <typename U, typename V>
    void printHelperR(TreeNode<TKey, pair<U, V>>*const& currRoot) const {
        if (currRoot != nullptr) {
            printHelperR(currRoot->_left);
            cout << currRoot->_key << '-' << currRoot->_value.first << '~' << currRoot->_value.second << '\n';
            printHelperR(currRoot->_right);
        }
    }
    */
};

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
    BinarySearchTree<int, Bus> _busDB;
};

// unused class EngRusDict
/*
using psi = pair<string, int>;
class EngRusDict {
public:
    EngRusDict() { }

    ~EngRusDict() { }

    // Возможно потом сделаю через initializer_list
    EngRusDict(int n) {
        string currWordEng, currWordRus;
        int currCounter;
        for (int _ = 0; _ < n; ++_) {
            cin >> currWordEng >> currWordRus >> currCounter;
            this->insertElement(currWordEng, currWordRus, currCounter);
        }
    }
    void insertWord(string wordEng, string wordRus) { _dictRoot.insert(wordEng, psi(wordRus, 1)); }

    void delWord(string wordEng) { _dictRoot.del(wordEng); }

    TreeNode<string, psi>* findWord(string wordEng) {
        TreeNode<string, psi>* wordPtr = _dictRoot.find(wordEng);

        //++(wordPtr->getValue().second);
        // Без нарушений инкапсуляции:
        IncCounter(wordPtr);

        return wordPtr;
    }

    void changeWord(string wordEng, string wordRus, int counter) {
        auto wordPtr = findWord(wordEng);
        if (wordPtr != nullptr) {
            wordPtr->setValue(psi(wordRus, counter));
        }
    }

    void findMostFreqUsedWordR(TreeNode<string, psi>* currBestWord) { 
        //if currBestWord
    }

    BinarySearchTree<string, psi> alghorthm() {
        BinarySearchTree<string, psi> dictRoot_copy = _dictRoot;


    }

    void IncCounter(TreeNode<string, psi>*& wordPtr) { 
        wordPtr->setValue(psi(wordPtr->getValue().first, (wordPtr->getValue().second) + 1)); 
    }

    void printDict() { _dictRoot.print(); }

private:
    // англ_(рус_счётчикОбращений)
    BinarySearchTree<string, psi> _dictRoot;

    void insertElement(string wordEng, string wordRus, int counter) { _dictRoot.insert(wordEng, psi(wordRus, counter)); }
};
*/

void test_BST() {
    BinarySearchTree<int, int> bst;

    cout << "30 20 10 40 50 5 15\n";

    bst.insert(30, 0);
    bst.insert(20, 0);
    bst.insert(10, 0);
    bst.insert(40, 0);
    bst.insert(50, 0);
    bst.insert(5, 0);
    bst.insert(15, 0);


    bst.print();

    cout << "*(bst.find(40)) = " << *(bst.find(40)) << '\n';
    cout << "Root = " << (bst.getRoot())->getKey() << '\n';
    cout << "bst.size() = " << bst.size() << '\n';
    
    cout << "bst.del(10);\n";
    bst.del(10);

    bst.print();
    cout << "bst.size() = " << bst.size() << '\n' << '\n';
}

void test_BusDB() {
    /*
    cat koshka 50
    dog sobaka 51
    dog sobaka2 52
    word slovo 52

    17 Alex 70 in_park
    92 Boba 51 abc
    11 Nikita 21 on_route
    */

    BusDB bdb1;
    bdb1.insertBus(32, Bus("Olog", 21, bus_status::on_route));
    bdb1.insertBus(17, Bus("Alex", 70, bus_status::in_park));
    bdb1.insertBus(92, Bus("Boba", 51));
    bdb1.insertBus(11, Bus("Nikita", 52, bus_status::on_route));
    bdb1.printBusDB();

    cout << "*(bdb1.findBus(32)) = " << *(bdb1.findBus(32)) << '\n';
    cout << "bdb1.size() = " << bdb1.size() << '\n';
    cout << "bdb1.toGo(17);\n";
    bdb1.toGo(17);
    bdb1.printBusDB();

    //BusDB bdb2(3);

    //bdb2.printBusDB();
}

void test_EngRusDict_NotUsed() { 
    /*
    EngRusDict d;

    d.insertWord("cat", "koshka");
    d.insertWord("dog", "sobaka");
    d.insertWord("dog", "sobaka2"); // nothing
    d.insertWord("word", "slovo");

    d.printDict();

    d.findWord("dog");

    d.printDict();

    d.changeWord("word", "lox", 52);

    d.printDict();

    //d.printDict();
*/
}

inline int rnd(int A, int B) { return A + rand() % (B - A + 1); }

int main() {
    // если что, INT_MAX = 2.147.483.647
    srand(time(0));

    cout << "--------------------- test_BST -------------------------------\n\n";
    test_BST();
    cout << "--------------------------------------------------------------\n\n";

    cout << "--------------------- test_BusDB -----------------------------\n\n";
    test_BusDB();
    cout << "--------------------------------------------------------------\n\n";

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
    vector<pair<int, Bus>>arrBuses_sorted = arrBuses;

    // Перемешиваем массив
    random_shuffle(arrBuses.begin(), arrBuses.end());

    // Перемешанный массив поэлеметно добавляем в BST
    for (int i = 0; i < arrBuses.size(); ++i) {
        bdb.insertBus(arrBuses[i].first, arrBuses[i].second);
    }

    // Вывод автобусов \/
    /*
    for (int i = 1; i < arrBuses.size(); ++i) {
        cout << arrBuses[i].first << " -- " << arrBuses[i].second << '\n';
    }

    cout << "*(bdb.getBusRoot()) = " << *(bdb.getBusRoot()) << '\n' << '\n';

    bdb.printBusDB();

    cout << "--------------------------------------------------------------\n\n";
    */
    // Вывод автобусов /\


    pair<int, Bus> toFind;
    auto start_i = chrono::high_resolution_clock::now();
    auto stop_i = chrono::high_resolution_clock::now();
    auto duration_i = chrono::duration_cast<chrono::microseconds>(stop_i - start_i);
    auto duration_total_linear = chrono::duration_cast<chrono::microseconds>(stop_i - start_i);
    auto duration_total_bst = chrono::duration_cast<chrono::microseconds>(stop_i - start_i);
    auto duration_total_binSearch = chrono::duration_cast<chrono::microseconds>(stop_i - start_i);
    auto l_i = 0;
    auto r_i = arrBuses_sorted.size() - 1;
    auto m_i = (l_i + r_i) / 2;
    auto flag = false;

    // Для усложнения, ищем '' случайных элементов
    for (int _ = 0; _ < 10000; ++_) {
        toFind = arrBuses[rnd(0, arrBuses.size())];

        //cout << _+1 << ") ";


        // Линейный поиск в массиве
        start_i = chrono::high_resolution_clock::now();
        for (const auto& curr : arrBuses) {
            if (curr == toFind) { 
                break; 
            }
        }
        stop_i = chrono::high_resolution_clock::now();
        duration_i = chrono::duration_cast<chrono::microseconds>(stop_i - start_i);
        duration_total_linear += duration_i;

        //cout << duration_i.count() << ' ';


        // Поиск в дереве
        start_i = chrono::high_resolution_clock::now();
        bdb.findBus(toFind.first);
        stop_i = chrono::high_resolution_clock::now();
        duration_i = chrono::duration_cast<chrono::microseconds>(stop_i - start_i);
        duration_total_bst += duration_i;

        //cout << duration_i.count() << ' ';


        // Бинарный поиск в массиве
        l_i = 0;
        r_i = arrBuses_sorted.size() - 1;
        flag = false;

        start_i = chrono::high_resolution_clock::now();
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
        stop_i = chrono::high_resolution_clock::now();
        duration_i = chrono::duration_cast<chrono::microseconds>(stop_i - start_i);
        duration_total_binSearch += duration_i;
        //cout << duration_i.count() << '\n';
    }

    cout << '\n';

    cout << duration_total_linear.count() << ' ' << duration_total_bst.count() << ' ' << duration_total_binSearch.count() << '\n';

    // бин. поиск быстрее чем бинарное дерево поиска быстрее чем линейный поиск
    
    return 0;
}