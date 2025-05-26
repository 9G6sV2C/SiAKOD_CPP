#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;


int __comparisonCounter; // счётчик сравнений
int __assignmentСounter; // счётчик присваиваний

#pragma region QuickSort
template <typename T>
int partition(vector<T>& v, int l, int r) {
    T pivot = v[(l + r) / 2]; // Пусть опорный по середине
    ++__assignmentСounter;

    while (l <= r) {
        ++__comparisonCounter;

        while (v[l] < pivot) {
            ++__comparisonCounter;

            ++l;
            ++__assignmentСounter;
        }
        while (v[r] > pivot) {
            ++__comparisonCounter;

            --r;
            ++__assignmentСounter;
        }

        ++__comparisonCounter;
        if (l <= r) {

            swap(v[l], v[r]);
            __assignmentСounter += 3;

            ++l;
            --r;
            __assignmentСounter += 2;

        }
    }
    return l;
}

template <typename T>
void QuickSort(vector<T>& v, int start, int end) {

    ++__comparisonCounter;
    if (start < end) {

        int rightStart = partition(v, start, end);
        ++__assignmentСounter;

        QuickSort(v, start, rightStart - 1);
        QuickSort(v, rightStart, end);
    }
}

template <typename T>
void QuickSort(vector<T>& v) {
    QuickSort(v, 0, v.size() - 1);
}

template <typename T>
void QuickSortCounter(vector<T>& v) {
    __comparisonCounter = 0;
    __assignmentСounter = 0;
    QuickSort(v);
    cout << "QuickSortCounter:\n";
    cout << "--- comparisons = " << __comparisonCounter << '\n';
    cout << "--- assignments = " << __assignmentСounter << '\n';
}
#pragma endregion

#pragma region HeapSort
// Восстановления свойства кучи
// Обмен знаениями между коренем поддререва и его бОльшим дочерним
template<typename T>
void heapify(vector<T>& v, int& n, int i) {
    int largest = i;   // Initialize largest as root
    int l = 2 * i + 1;
    int r = 2 * i + 2;
    __assignmentСounter += 3;


    // If left child is larger than root
    __comparisonCounter += 2;
    if (l < n && v[l] > v[largest]) {

        ++__assignmentСounter;
        largest = l;
    }

    // If right child is larger than largest so far
    __comparisonCounter += 2;
    if (r < n && v[r] > v[largest]) {

        ++__assignmentСounter;
        largest = r;

    }

    // If largest is not root
    ++__comparisonCounter;
    if (largest != i) {

        swap(v[i], v[largest]);
        __assignmentСounter += 3;

        // Рекурсивное восстановление кучи у поддерева
        heapify(v, n, largest);
    }
}

template <typename T>
void HeapSort(vector<T>& v) {
    int n = v.size();
    ++__assignmentСounter;

    // Пострение кучи (перегрупировка массива)
    for (int i = n / 2 - 1; i >= 0; --i) {
        ++__comparisonCounter;

        heapify(v, n, i);
    }

    // Один за другим извлекаем элементы из кучи
    for (int i = n - 1; i > 0; --i) {
        ++__comparisonCounter;

        // Перемещаем текущий корень в конец
        swap(v[0], v[i]);
        __assignmentСounter += 3;

        // вызываем heapify на уменьшенной куче
        heapify(v, i, 0);
    }
}

template <typename T>
void HeapSortCounter(vector<T>& v) {
    __comparisonCounter = 0;
    __assignmentСounter = 0;
    HeapSort(v);
    cout << "HeapSortCounter:\n";
    cout << "=== comparisons = " << __comparisonCounter << '\n';
    cout << "=== assignments = " << __assignmentСounter << '\n';
}
#pragma endregion

class wallPanel {
public:
    int _width;
    int _length;
    int _numOf;
    int _price;

    bool operator<(const wallPanel& other) const { return _price < other._price; }
    bool operator>(const wallPanel& other) const { return _price > other._price; }
};

ostream& operator<<(ostream& os, const wallPanel& wp) {
    return os << "( " << wp._width << ' ' << wp._length << ' ' << wp._numOf << ' ' << wp._price << " )";
}

template <typename T>
void PrintArraySpace(const vector<T>& arr) {
    for (const T& el : arr) {
        cout << el << ' ';
    }
}

template <typename T>
void PrintArrayNewLine(const vector<T>& arr) {
    for (const T& el : arr) {
        cout << el << '\n';
    }
}

inline int rnd(int A, int B) { return A + rand() % (B - A + 1); }

vector<int> CreateRandomIntArray(int n, int A, int B) {
    vector<int> arr;

    for (int i = 0; i < n; ++i) {
        arr.push_back(rnd(A, B));
    }

    random_shuffle(arr.begin(), arr.end());

    return arr;
}

vector<wallPanel> CreateRandomWallPanelArray(int n) {
    vector<wallPanel> arr;

    for (int i = 0; i < n; ++i) {
        arr.push_back({rnd(10, 99), rnd(10,99), rnd(10,50), rnd(1000, 9999)});
    }

    random_shuffle(arr.begin(), arr.end());

    return arr;
}

void demo() {
    for (int _ = 0; _ < 10; ++_) {
        vector<int> arr = CreateRandomIntArray(10, -10000, 20000);
        PrintArraySpace(arr);
        cout << "- orig\n";

        vector<int> arr1 = arr;
        QuickSortCounter(arr1);
        PrintArraySpace(arr1);
        cout << "- QuickSort\n";

        vector<int> arr2 = arr;
        HeapSortCounter(arr2);
        PrintArraySpace(arr2);
        cout << "- HeapSort\n";

        vector<int> arr3 = arr;
        sort(arr3.begin(), arr3.end());
        PrintArraySpace(arr3);
        cout << "- sort\n";

        if (arr1 != arr2) cout << "arr1 != arr2 !!!\n";
        if (arr1 != arr3) cout << "arr1 != arr3 !!!\n";
        if (arr2 != arr3) cout << "arr2 != arr3 !!!\n";

        cout << "\n\n";
    }
}

int main() {
    srand(time(0));

    //demo();

    int N = 5000;

    vector<wallPanel> arr_orig = CreateRandomWallPanelArray(10000);

    //PrintArrayNewLine(arr_orig);

    vector<wallPanel> arr1 = arr_orig;
    QuickSortCounter(arr1);

    vector<wallPanel> arr2 = arr_orig;
    HeapSortCounter(arr2);


    /*
    cout << "------------------\n";
    PrintArrayNewLine(arr_orig);
    cout << "------------------\n";
    for (const auto& el : arr_orig) {
        if (el._price >= N) {
            cout << el << '\n';
        }
    }
    */

    return 0;
}
