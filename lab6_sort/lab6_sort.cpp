#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

#pragma region QuickSort
template <typename T>
int partition(vector<T>& v, int l, int r) {
    int pivot = v[(l + r) / 2]; // Пусть опорный по середине
    while (l <= r) {
        while (v[l] < pivot) ++l;
        while (v[r] > pivot) --r;
        if (l <= r) {
            swap(v[l], v[r]);
            ++l;
            --r;
        }
    }
    return l;
}

template <typename T>
void QuickSort(vector<T>& v, int start, int end) {
    if (start < end) {
        int rightStart = partition(v, start, end);
        QuickSort(v, start, rightStart - 1);
        QuickSort(v, rightStart, end);
    }
}

template <typename T>
void QuickSort(vector<T>& v) {
    QuickSort(v, 0, v.size() - 1);
}
#pragma endregion

#pragma region HeapSort
template<typename T>
void heapify(vector<T>& v, int& n, int i) {
    int largest = i;   // Initialize largest as root
    int l = 2 * i + 1;
    int r = 2 * i + 2;

    // If left child is larger than root
    if (l < n && v[l] > v[largest])
        largest = l;

    // If right child is larger than largest so far
    if (r < n && v[r] > v[largest])
        largest = r;

    // If largest is not root
    if (largest != i) {
        swap(v[i], v[largest]);

        // Recursively heapify the affected sub-tree
        heapify(v, n, largest);
    }
}

template <typename T>
void HeapSort(vector<T>& v) {
    int n = v.size();

    // Пострение кучи (перегрупировка массива)
    for (int i = n / 2 - 1; i >= 0; --i) {
        heapify(v, n, i);
    }

    // Один за другим извлекаем элементы из кучи
    for (int i = n - 1; i > 0; --i) {
        // Перемещаем текущий корень в конец
        swap(v[0], v[i]);

        // вызываем heapify на уменьшенной куче
        heapify(v, i, 0);
    }
}
#pragma endregion

template <typename T>
void PrintArray(const vector<T>& arr) {
    for (const T& el : arr) {
        cout << el << ' ';
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

int main() {
    for (int _ = 0; _ < 1000; ++_) {
        vector<int> arr = CreateRandomIntArray(10000, -10000, 20000);
        //PrintArray(arr);
        //cout << "- orig\n";

        vector<int> arr1 = arr;
        QuickSort(arr1);
        //PrintArray(arr1);
        //cout << "- QuickSort\n";

        vector<int> arr2 = arr;
        HeapSort(arr2);
        //PrintArray(arr2);
        //cout << "- HeapSort\n";

        vector<int> arr3 = arr;
        sort(arr3.begin(), arr3.end());
        //PrintArray(arr3);
        //cout << "- sort\n";

        if (arr1 != arr2) cout << "arr1 != arr2 !!!\n";
        if (arr1 != arr3) cout << "arr1 != arr3 !!!\n";
        if (arr2 != arr3) cout << "arr2 != arr3 !!!\n";

        cout << _ << '\n';
    }

    return 0;
}
