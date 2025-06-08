#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <string>
#include <chrono>

using namespace std;
using namespace std::chrono;

const size_t CHUNK_SIZE = 1000000000; // Размер чанка (1 млрд чисел)

// Функция для сохранения вектора в файл
void saveChunkToFile(const vector<int>& chunk, const string& filename) {
    ofstream outFile(filename);
    if (!outFile) {
        cerr << "Error opening file for writing: " << filename << endl;
        return;
    }
    for (int num : chunk) {
        outFile << num << '\n';
    }
    outFile.close();
}

// Функция для чтения чанка из файла
vector<int> readChunkFromFile(const string& filename) {
    vector<int> chunk;
    ifstream inFile(filename);
    if (!inFile) {
        cerr << "Error opening file for reading: " << filename << endl;
        return chunk;
    }
    chunk.reserve(CHUNK_SIZE);
    int num;
    while (inFile >> num) {
        chunk.push_back(num);
    }
    return chunk;
}

// Функция для слияния двух файлов
void mergeTwoFiles(const string& file1, const string& file2, const string& outputFile) {
    ifstream in1(file1), in2(file2);
    ofstream out(outputFile);

    int num1, num2;
    bool hasNum1 = (bool)(in1 >> num1);
    bool hasNum2 = (bool)(in2 >> num2);

    while (hasNum1 && hasNum2) {
        if (num1 <= num2) {
            out << num1 << '\n';
            hasNum1 = (bool)(in1 >> num1);
        }
        else {
            out << num2 << '\n';
            hasNum2 = (bool)(in2 >> num2);
        }
    }

    while (hasNum1) {
        out << num1 << '\n';
        hasNum1 = (bool)(in1 >> num1);
    }
    while (hasNum2) {
        out << num2 << '\n';
        hasNum2 = (bool)(in2 >> num2);
    }
}

// Основная функция сортировки
void externalTextFileSort(const string& inputFile, const string& outputFile) {
    auto start = high_resolution_clock::now();

    ifstream in(inputFile);
    if (!in) {
        cerr << "Failed to open input file: " << inputFile << endl;
        return;
    }

    vector<string> chunkFiles;
    vector<int> chunk;
    chunk.reserve(CHUNK_SIZE);
    int num;
    size_t totalNumbers = 0;

    cout << "Phase 1: Creating sorted chunks..." << endl;

    while (in >> num) {
        chunk.push_back(num);
        if (chunk.size() == CHUNK_SIZE) {
            sort(chunk.begin(), chunk.end());
            string chunkFilename = "chunk_" + to_string(chunkFiles.size()) + ".txt";
            saveChunkToFile(chunk, chunkFilename);
            chunkFiles.push_back(chunkFilename);
            totalNumbers += chunk.size();
            chunk.clear();
            chunk.reserve(CHUNK_SIZE);

            if (chunkFiles.size() % 10 == 0) {
                cout << "Chunks created: " << chunkFiles.size() << endl;
            }
        }
    }

    // Последний чанк
    if (!chunk.empty()) {
        sort(chunk.begin(), chunk.end());
        string chunkFilename = "chunk_" + to_string(chunkFiles.size()) + ".txt";
        saveChunkToFile(chunk, chunkFilename);
        chunkFiles.push_back(chunkFilename);
        totalNumbers += chunk.size();
    }

    cout << "Phase 2: Merge" << chunkFiles.size() << "chunks..." << endl;

    while (chunkFiles.size() > 1) {
        vector<string> newChunkFiles;
        for (size_t i = 0; i < chunkFiles.size(); i += 2) {
            if (i + 1 < chunkFiles.size()) {
                string mergedFilename = "merged_" + to_string(newChunkFiles.size()) + ".txt";
                mergeTwoFiles(chunkFiles[i], chunkFiles[i + 1], mergedFilename);
                newChunkFiles.push_back(mergedFilename);
                // Удаление временных файлов закомментировано
                // remove(chunkFiles[i].c_str());
                // remove(chunkFiles[i + 1].c_str());
            }
            else {
                newChunkFiles.push_back(chunkFiles[i]);
            }
        }
        chunkFiles = newChunkFiles;
        cout << "There are still chunks left: " << chunkFiles.size() << endl;
    }




    // Финальное сохранение (без удаления последнего чанка)
    if (!chunkFiles.empty()) {
        if (rename(chunkFiles[0].c_str(), outputFile.c_str()) != 0) {
            ifstream src(chunkFiles[0]);
            ofstream dst(outputFile);
            dst << src.rdbuf();
        }
        // Удаление закомментировано
        // remove(chunkFiles[0].c_str());
    }

    auto end = high_resolution_clock::now();
    cout << "Sorting is complete. Processed " << totalNumbers << " numbers for "
        << duration_cast<seconds>(end - start).count() << " seconds." << endl;
    cout << "Temporary files are saved on disk." << endl;
}

int main() {
    setlocale(LC_ALL, "RUS");
    const string inputFile = "proba.txt";
    const string outputFile = "SortedFile.txt";

    externalTextFileSort(inputFile, outputFile);
    return 0;
}
