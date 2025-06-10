#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <string>
#include <chrono>
#include <random>

using namespace std;
using namespace std::chrono;

void generate_large_file() {
    // Инициализация генератора случайных чисел
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> dist(0, INT_MAX);

    const long long TARGET_SIZE = 1024 * 1024 * 1024; // 1 ГБ в байтах

    ofstream outfile("proba.txt");
    if (!outfile) {
        cerr << "Couldn't open the file for writing!" << '\n';
        return;
    }


    long long current_size = 0;
    int counter = 0;

    while (current_size < TARGET_SIZE) {
        int num = dist(gen); // Генерируем случайное число
        outfile << num << '\n'; // Записываем число с новой строки

        if (!outfile) {
            cerr << "Error writing to the file!" << '\n';
            return;
        }

        // Приблизительный расчёт размера (каждое число + перевод строки)
        current_size += to_string(num).length() + 1;

        // Выводим прогресс каждый 1 миллион записей
        if (++counter % 1000000 == 0) {
            cout << "Wroten: " << (current_size / 1048576) << " MB" << '\n';
        }
    }

    outfile.close();
    cout << "The file 'proba.txt 'successfully created. Size: "
         << (current_size / 1048576) << " MB." << '\n';
}


const size_t CHUNK_SIZE = 10'000'000; // Размер чанка (1 млрд чисел)

// Функция для сохранения вектора в файл
void saveChunkToFile(const vector<int>& chunk, const string& filename) {
    ofstream outFile(filename);
    if (!outFile) {
        cerr << "Error opening file for writing: " << filename << '\n';
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
        cerr << "Error opening file for reading: " << filename << '\n';
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
        cerr << "Failed to open input file: " << inputFile << '\n';
        return;
    }

    vector<string> chunkFiles;
    vector<int> chunk;
    chunk.reserve(CHUNK_SIZE);
    int num;
    size_t totalNumbers = 0;

    cout << "Phase 1: Creating sorted chunks..." << '\n';

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
                cout << "Chunks created: " << chunkFiles.size() << '\n';
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

    cout << "Phase 2: Merge" << chunkFiles.size() << "chunks..." << '\n';

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
        cout << "There are still chunks left: " << chunkFiles.size() << '\n';
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
        << duration_cast<milliseconds>(end - start).count() << " milliseconds." << '\n';
    cout << "Temporary files are saved on disk." << '\n';
}

int main() {
    const string inputFile = "_INPUT.txt";
    const string outputFile = "SortedFile.txt";

    externalTextFileSort(inputFile, outputFile);
    return 0;
}
