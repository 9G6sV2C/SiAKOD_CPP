//#include <iostream>
//#include <fstream>
//#include <string>
//#include <climits>
//
//using namespace std;
//
//inline int rnd(int minVal, int maxVal) { return minVal + rand() % (maxVal - minVal + 1); }
//
//#pragma region Simple
//void SimpleSplitFile(const string& inputFileName, const string& outputFileName1, const string& outputFileName2, int runSize) {
//    ifstream input(inputFileName);
//    ofstream output1(outputFileName1);
//    ofstream output2(outputFileName2);
//
//    int num;
//    bool toOutput1 = true;
//    int count = 0;
//
//    while (input >> num) {
//        (toOutput1) ? output1 << num << '\n' : output2 << num << '\n';
//
//        ++count;
//        if (count == runSize) {
//            toOutput1 = !toOutput1;
//            count = 0;
//        }
//    }
//
//    input.close();
//    output1.close();
//    output2.close();
//}
//
//void SimpleMergeFiles(const string& inputFile1, const string& inputFile2, const string& outputFile, int runSize) {
//    ifstream input1(inputFile1);
//    ifstream input2(inputFile2);
//    ofstream output(outputFile);
//
//    int num1, num2;
//    bool hasNum1 = bool(input1 >> num1);
//    bool hasNum2 = bool(input1 >> num2);
//
//    int count1 = 0, count2 = 0;
//
//    while (hasNum1 && hasNum2) {
//        if (count1 < runSize && count2 < runSize) {
//            if (num1 <= num2) {
//                output << num1 << '\n';
//                hasNum1 = bool(input1 >> num1);
//                ++count1;
//            }
//            else {
//                output << num2 << '\n';
//                hasNum2 = bool(input2 >> num2);
//                ++count2;
//            }
//        }
//        else {
//            // Дописываем оставшиеся элементы из текущего блока
//            while (count1 < runSize && hasNum1) {
//                output << num1 << '\n';
//                hasNum1 = bool(input1 >> num1);
//                count1++;
//            }
//            while (count2 < runSize && hasNum2) {
//                output << num2 << '\n';
//                hasNum2 = bool(input2 >> num2);
//                count2++;
//            }
//            count1 = 0;
//            count2 = 0;
//        }
//    }
//
//    // Дописываем оставшиеся элементы из любого файла
//    while (hasNum1) {
//        output << num1 << '\n';
//        hasNum1 = bool(input1 >> num1);
//    }
//    while (hasNum2) {
//        output << num2 << '\n';
//        hasNum2 = bool(input2 >> num2);
//    }
//
//    input1.close();
//    input2.close();
//    output.close();
//}
//
//void SimpleExternalSort(const string& inputFileName, int n) {
//    const string tempf1 = "tempF1.txt";
//    const string tempf2 = "tempF2.txt";
//
//    ifstream input(inputFileName);
//    if (!input) {
//        cerr << "Error: Cannot open input file." << '\n';
//        exit(1);
//    }
//
//    int runSize = 1;
//    bool isSorted, isDone = false;
//
//    while (runSize < n) {
//        // Разделяем файл на два временных файла
//        SimpleSplitFile(inputFileName, tempf1, tempf2, runSize);
//
//        // Сливаем временные файлы обратно в исходный
//        SimpleMergeFiles(tempf1, tempf2, inputFileName, runSize);
//
//        runSize *= 2;
//    }
//
//    // Удаляем временные файлы
//    remove(tempf1.c_str());
//    remove(tempf2.c_str());
//}
//#pragma endregion
//
//#pragma region Natural
//void NaturalSplitFile(const string& inputFile, const string& outputFile1, const string& outputFile2) {
//    ifstream input(inputFile);
//    ofstream out1(outputFile1);
//    ofstream out2(outputFile2);
//
//    bool writeToFirst = true;
//    int curr, prev;
//
//    if (input >> prev) {
//        out1 << prev << '\n';
//
//        while (input >> curr) {
//            if (curr < prev) {
//                writeToFirst = !writeToFirst;
//            }
//
//            if (writeToFirst) {
//                out1 << curr << '\n';
//            }
//            else {
//                out2 << curr << '\n';
//            }
//
//            prev = curr;
//        }
//    }
//}
//
//void NaturalMergeFiles(const string& inputFile1, const string& inputFile2, const string& outputFile) {
//    ifstream input1(inputFile1);
//    ifstream input2(inputFile2);
//    ofstream output(outputFile);
//
//    int num1, num2;
//    bool isNotEmpty1 = bool(input1 >> num1);
//    bool isNotEmpty2 = bool(input2 >> num2);
//
//    while (isNotEmpty1 && isNotEmpty2) {
//        if (num1 <= num2) {
//            output << num1 << '\n';
//            isNotEmpty1 = bool(input1 >> num1);
//        }
//        else {
//            output << num2 << '\n';
//            isNotEmpty2 = bool(input2 >> num2);
//        }
//    }
//
//    while (isNotEmpty1) {
//        output << num1 << '\n';
//        isNotEmpty1 = bool(input1 >> num1);
//    }
//
//    while (isNotEmpty2) {
//        output << num2 << '\n';
//        isNotEmpty2 = bool(input2 >> num2);
//    }
//}
//
//void NaturalExternalSort(const string& filename) {
//    const string tempFile1 = "temp1.txt";
//    const string tempFile2 = "temp2.txt";
//
//    ifstream input(filename);
//    int prev, curr;
//    bool isSorted = false;
//
//    while (!isSorted) {
//        input.close();
//        input.open(filename);
//
//        // Проверка на отсортированность
//        isSorted = true;
//        if (input >> prev) {
//            while (input >> curr) {
//                if (curr < prev) {
//                    isSorted = false;
//                    break;
//                }
//                prev = curr;
//            }
//        }
//        input.close();
//
//        if (isSorted) break;
//
//        NaturalSplitFile(filename, tempFile1, tempFile2);
//        NaturalMergeFiles(tempFile1, tempFile2, filename);
//    }
//
//    // Удаляем временные файлы
//    remove(tempFile1.c_str());
//    remove(tempFile2.c_str());
//}
//#pragma endregion
//
//
//// Генерация исходного файла со случайными числами
//void FillFileWithRandNums(const string& fileName, int count, int minVal, int maxVal) {
//    ofstream out(fileName);
//    if (!out.is_open()) {
//        cerr << "Error: Cannot open input file." << '\n';
//        exit(1);
//    }
//
//    for (int i = 0; i < count; ++i) {
//        out << rnd(minVal, maxVal) << '\n';
//    }
//
//    out.close();
//}
//
//int main() {
//    srand(time(0));
//
//    int n = 20;
//    string inputFileName = "f.txt";
//
//    // Предварительно очищаем файлы
//    fstream clear_file1(inputFileName, ios::out);
//    clear_file1.close();
//
//    FillFileWithRandNums(inputFileName, 20, 10, 99);
//
//    ifstream f;
//
//    f.open(inputFileName);
//    for (int curr; f >> curr;) { cout << curr << '\n'; }
//    f.close();
//    cout << "~~~~~~\n";
//
//    //SimpleExternalSort(inputFileName, n);
//    NaturalExternalSort(inputFileName);
//
//    f.open(inputFileName);
//    for (int curr; f >> curr;) { cout << curr << '\n'; }
//    f.close();
//
//    return 0;
//}