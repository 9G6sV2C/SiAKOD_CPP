#include <iostream>
#include <fstream>
#include <string>

using namespace std;

void SimpleMergeFiles(const string& inputFile1, const string& inputFile2, const string& outputFile, int runSize) {
    ifstream input1(inputFile1);
    ifstream input2(inputFile2);
    ofstream output(outputFile);

    int num1, num2;

    bool isNotEmpty1 = bool(input1 >> num1);
    bool isNotEmpty2 = bool(input1 >> num2);

    int count1 = 0, count2 = 0;

    while (isNotEmpty1 && isNotEmpty2) {
        if (count1 < runSize && count2 < runSize) {
            if (num1 <= num2) {
                output << num1 << '\n';
                isNotEmpty1 = bool(input1 >> num1);
                ++count1;
            }
            else {
                output << num2 << '\n';
                isNotEmpty2 = bool(input2 >> num2);
                ++count2;
            }
        }
        else {
            // Дописываем оставшиеся элементы из текущего блока
            while (count1 < runSize && isNotEmpty1) {
                output << num1 << '\n';
                isNotEmpty1 = bool(input1 >> num1);
                count1++;
            }
            while (count2 < runSize && isNotEmpty2) {
                output << num2 << '\n';
                isNotEmpty2 = bool(input2 >> num2);
                count2++;
            }
            count1 = 0;
            count2 = 0;
        }
    }

    // Дописываем оставшиеся элементы из любого файла
    while (isNotEmpty1) {
        output << num1 << '\n';
        isNotEmpty1 = bool(input1 >> num1);
    }
    while (isNotEmpty2) {
        output << num2 << '\n';
        isNotEmpty2 = bool(input2 >> num2);
    }

    input1.close();
    input2.close();
    output.close();
}
















void NaturalMergeFiles(const string& inputFile1, const string& inputFile2, const string& outputFile) {
    ifstream input1(inputFile1);
    ifstream input2(inputFile2);
    ofstream output(outputFile);

    int num1, num2;
    bool isNotEmpty1 = bool(input1 >> num1);
    bool isNotEmpty2 = bool(input2 >> num2);

    while (isNotEmpty1 && isNotEmpty2) {
        if (num1 <= num2) {
            output << num1 << '\n';
            isNotEmpty1 = bool(input1 >> num1);
        }
        else {
            output << num2 << '\n';
            isNotEmpty2 = bool(input2 >> num2);
        }
    }

    while (isNotEmpty1) {
        output << num1 << '\n';
        isNotEmpty1 = bool(input1 >> num1);
    }

    while (isNotEmpty2) {
        output << num2 << '\n';
        isNotEmpty2 = bool(input2 >> num2);
    }
}


