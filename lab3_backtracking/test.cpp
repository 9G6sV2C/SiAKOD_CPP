//#include <iostream>
//#include <vector>
//#include <string>
//
//using namespace std;
//
//void findMaxChain(const vector<string>& words, vector<bool>& used, const string& lastWord,
//                  vector<string>& currChain, vector<string>& maxChain) {
//
//    // Проверяем, не нашли ли мы цепочку длиннее текущей максимальной
//    if (currChain.size() > maxChain.size()) {
//        maxChain = currChain;
//    }
//
//    // Перебираем все слова
//    for (size_t i = 0; i < words.size(); ++i) {
//        // Если слово еще не использовано и подходит по условию (первая буква совпадает с последней предыдущего)
//        if (!used[i] && (lastWord.empty() || words[i][0] == lastWord.back())) {
//            // Делаем выбор
//            // Помечаем слово как использованное
//            used[i] = true;
//            // Добавляем в текущую цепочку
//            currChain.push_back(words[i]);
//
//            // Рекурсивно ищем продолжение цепочки для текущего слова
//            findMaxChain(words, used, words[i], currChain, maxChain);
//
//            // Отменяем выбор
//            currChain.pop_back();
//            used[i] = false;
//        }
//    }
//}
//
//vector<string> getMaxWordChain(const vector<string>& words) {
//    vector<string> maxChain;
//    vector<string> currChain;
//    vector<bool> used(words.size(), false);
//
//    findMaxChain(words, used, "", currChain, maxChain);
//
//    return maxChain;
//}
//
//int main() {
//    // Пример использования
//    vector<string> words = {"apple", "elephant", "tiger", "rabbit", "tea", "eagle"};
//
//    vector<string> maxChain = getMaxWordChain(words);
//
//    cout << "Max chain length: " << maxChain.size() << '\n';
//    cout << "Chain: ";
//    for (const auto& word : maxChain) {
//        cout << word << " ";
//    }
//    cout << '\n';
//
//    return 0;
//}