#include <iostream>
#include <vector>

using namespace std;

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

template <typename T>
void HeapSort(vector<T>& v) {

}

template <typename T>
void printArray(const vector<T>& arr) {
    for (const T& el : arr) {
        cout << el << ' ';
    }
    cout << '\n';
}

int main() {
    vector<int> arr = {10, 7, 8, 9, 1, 5};
    printArray(arr);

    QuickSort(arr);

    printArray(arr);

    return 0;
}
