#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <windows.h>
#include <conio.h>
#include <queue>

using namespace std;

struct Question {
    string text;
    string answer;
    string explanation;
    string image;
};

void clearScreen() {
    system("cls");
}

void closeImage() {
    system("taskkill /F /IM Microsoft.Photos.exe > nul 2>&1");
    system("taskkill /F /IM mspaint.exe > nul 2>&1");
    system("taskkill /F /IM dllhost.exe > nul 2>&1");
}

void showImage(string path) {
    if (path.empty()) return;
    system(("start \"\" \"" + path + "\"").c_str());
}

string cp866_to_1251(const string& str) {
    if (str.empty()) return "";

    int size_needed = MultiByteToWideChar(866, 0, str.c_str(), (int)str.length(), NULL, 0);
    if (size_needed == 0) return str;

    wstring wstr(size_needed, 0);
    MultiByteToWideChar(866, 0, str.c_str(), (int)str.length(), &wstr[0], size_needed);

    size_needed = WideCharToMultiByte(1251, 0, wstr.c_str(), (int)wstr.length(), NULL, 0, NULL, NULL);
    if (size_needed == 0) return str;

    string result(size_needed, 0);
    WideCharToMultiByte(1251, 0, wstr.c_str(), (int)wstr.length(), &result[0], size_needed, NULL, NULL);

    return result;
}

string toLower(string str) {
    for (char& c : str) {
        unsigned char uc = static_cast<unsigned char>(c);
        
        if (uc >= 128 && uc <= 159) { 
            c = static_cast<char>(uc + 32); 
        }
        else if (uc == 240) { 
            c = static_cast<char>(241); 
        }
        else if (c >= 'A' && c <= 'Z') {
            c = static_cast<char>(c + ('a' - 'A'));
        }
    }
    return str;
}

string trim(const string& str) {
    size_t start = str.find_first_not_of(" \t\n\r");
    if (start == string::npos) return "";
    size_t end = str.find_last_not_of(" \t\n\r");
    return str.substr(start, end - start + 1);
}

bool checkAnswer(string userAnswer, string correctAnswer) {
    userAnswer = trim(toLower(userAnswer));
    correctAnswer = trim(toLower(correctAnswer));

    if (userAnswer == correctAnswer) return true;

    if (correctAnswer == "избушка на курьих ножках") {
        if (userAnswer == "изба на курьих ножках" ||
            userAnswer == "изба бабы яги" ||
            userAnswer == "избушка бабы яги") {
            return true;
        }
    }

    if (correctAnswer == "остров святой елены") {
        if (userAnswer == "остров св.елены" ||
            userAnswer == "остров св елены" ||
            userAnswer == "остров святой еллены" ||
            userAnswer == "остров елены") return true;
    }

    if (correctAnswer == "позвонок птерозавра") {
        if (userAnswer == "позвонок птеранодона" ||
            userAnswer == "позвонки птеранодона" ||
            userAnswer == "позвонки птерозавра" ||
            userAnswer == "шейный позвонок птерозавра") return true;
    }

    if (correctAnswer == "флеминг") {
        if (userAnswer == "александр флеминг" ||
            userAnswer == "флемминг" ||
            userAnswer == "александр флемминг") return true;
    }

    if (correctAnswer == "лупа") {
        if (userAnswer == "луппа" ||
            userAnswer == "увеличительное стекло" ||
            userAnswer == "увеличительное стёкло") return true;
    }

    return false;
}


string getInputWithTimer(int seconds, bool& timeout, bool& skipped) {
    timeout = false;
    skipped = false;
    string answer;

   
    while (_kbhit()) _getch();

    auto startTime = chrono::steady_clock::now();

    while (chrono::duration_cast<chrono::seconds>(
        chrono::steady_clock::now() - startTime).count() < seconds) {

        if (_kbhit()) {
            int ch = _getch();

           
            if (ch == 0 || ch == 224) {
                _getch(); 
                continue;
            }

           
            if (ch == '\r' || ch == '\n') {
                cout << endl;
                return answer;
            }
            
            else if (ch == '\b' || ch == 127) {
                if (!answer.empty()) {
                    answer.pop_back();
                    cout << "\b \b" << flush;
                }
            }
            
            else if (ch == 27) {
                skipped = true;
                cout << endl;
                return "";
            }
           
            else if (ch >= 32 && ch <= 126) {
                answer += static_cast<char>(ch);
                cout << static_cast<char>(ch) << flush;
            }
            
            else if (ch >= 128 && ch <= 255) {
                answer += static_cast<char>(ch);
                cout << static_cast<char>(ch) << flush;
            }
        }

        this_thread::sleep_for(chrono::milliseconds(50));
    }

   
    timeout = true;
    cout << endl;

    
    while (_kbhit()) _getch();

    return answer;
}


void waitForEnter() {
    cout << "Нажмите Enter для продолжения...";
    while (true) {
        if (_kbhit()) {
            int ch = _getch();
            if (ch == '\r' || ch == '\n') {
                cout << endl;
                break;
            }
        }
        this_thread::sleep_for(chrono::milliseconds(50));
    }
}


void pauseWithCountdown(int seconds) {
    cout << "\n";
    for (int i = seconds; i > 0; i--) {
        cout << "\rПереход к следующему вопросу через " << i << " секунд..." << flush;
        this_thread::sleep_for(chrono::seconds(1));
    }
    cout << "\r" << string(60, ' ') << "\r" << flush;
}

int main() {
    
    SetConsoleOutputCP(1251);
    SetConsoleCP(866);

    vector<Question> questions = {
        {"На юмористической картинке ОНА стала причиной спора архитекторов и орнитологов. Назовите ЕЁ.",
         "избушка на курьих ножках",
         "Архитекторы и орнитологи не могли решить, чей это объект исследования.",
         "izba.jpg"},

        {"В начале девятнадцатого века шотландский замок Дамбартон был хорошо укреплён и расположен в уединённом месте. Дамбартон рассматривался как один из вариантов, но предпочли ЕГО. Назовите ЕГО тремя словами.",
         "остров святой елены",
         "Замок рассматривался как кандидат на место заточения Наполеона. Далекий остров оказался более надежным.",
         "ostrov_sv_elen.jpg"},

        {"Станислав Дробышевский рассказывает, что акулы-мако способны выпрыгивать из воды на несколько метров в высоту. Зуб мезозойской родственницы мако был найден застрявшим между третьим и четвёртым ИКСАМИ. Назвать ИКС двумя словами на одну букву.",
         "позвонок птерозавра",
         "Мако могут выпрыгивать из воды на 6 метров. Скорее всего, мезозойские мако обладали теми же способностями, поскольку зуб одной из них застрял между шейными позвонками летящего птеранодона.",
         "Pteranodon_reconstruction.jpg"},

        {"Статья под названием 'Человек, который не любил мыть посуду' посвящена нобелевскому лауреату. Написать его фамилию.",
         "флеминг",
         "На вовремя не вымытой лабораторной посуде со стафилококками Александр Флеминг обнаружил плесень",
         "fleming.jpg"},

        {"В Великобритании выпустили монету, посвящённую известному персонажу. На монете написано множество произведений. Предполагается, что обладатель монеты уподобится этому персонажу и воспользуется ЕЕЙ. Чем же?",
         "лупа",
         "Монета посвящена Шерлоку Холмсу. На монете выгравированы названия произведений о нём. Шерлок Холмс часто пользовался лупой.",
         "pence_holms.jpg"},
    };

    int score = 0;
    int total = static_cast<int>(questions.size());

    clearScreen();
    cout << "\n========================================\n";
    cout << "     ИНТЕЛЛЕКТУАЛЬНАЯ ВИКТОРИНА \"Что? Где? Когда?\"\n";
    cout << "========================================\n\n";
    cout << "Всего вопросов: " << total << "\n";
    cout << "На каждый вопрос дается по 30 секунд\n";
    cout << "\nПодсказки:\n";
    cout << "  • Backspace - удаление символа\n";
    cout << "  • Esc - пропуск вопроса\n";
    cout << "  • Enter - подтверждение ответа\n\n";

    waitForEnter();

    for (int i = 0; i < total; i++) {
        clearScreen();

        cout << "\n========== Вопрос " << (i + 1) << " из " << total << " ==========\n\n";
        cout << questions[i].text << "\n\n";
        cout << "У вас 30 секунд!\n";
        cout << "Ваш ответ: " << flush;

        bool timeout = false;
        bool skipped = false;
        string userAnswer = getInputWithTimer(30, timeout, skipped);

        clearScreen();

        
        if (skipped) {
            cout << "\n========================================\n";
            cout << "ВОПРОС ПРОПУЩЕН\n";
            cout << "========================================\n\n";
        }
        else if (timeout && userAnswer.empty()) {
            cout << "\n========================================\n";
            cout << "ВРЕМЯ ВЫШЛО!\n";
            cout << "========================================\n\n";
        }
        else if (timeout && !userAnswer.empty()) {
            cout << "\n========================================\n";
            cout << "Время вышло, но ваш ответ принят!\n";
            cout << "========================================\n\n";
        }

        
        if (!skipped) {
            string normalizedAnswer = cp866_to_1251(userAnswer);

            if (checkAnswer(normalizedAnswer, questions[i].answer)) {
                cout << "ПРАВИЛЬНО! +1 балл\n\n";
                score++;
            }
            else {
                cout << "НЕПРАВИЛЬНО!\n\n";
            }
        }

        
        cout << "ПРАВИЛЬНЫЙ ОТВЕТ: " << questions[i].answer << "\n";
        cout << "ПОЯСНЕНИЕ: " << questions[i].explanation << "\n\n";

        
        if (!questions[i].image.empty()) {
            cout << "Открывается изображение: " << questions[i].image << "\n";
            showImage(questions[i].image);

          
            cout << "\nВремя на изучение изображения: 8 секунд\n";
            for (int j = 8; j > 0; j--) {
                cout << "\r   Изображение закроется через " << j << " секунд..." << flush;
                this_thread::sleep_for(chrono::seconds(1));
            }
            cout << "\r" << string(50, ' ') << "\r";

            closeImage();
            cout << "Изображение закрыто.\n";
        }

        
        if (i < total - 1) {
            
            pauseWithCountdown(8);
            cout << "\n⚡ Переходим к следующему вопросу!\n";
            this_thread::sleep_for(chrono::seconds(1));
        }
    }

    
    clearScreen();
    cout << "\n========================================\n";
    cout << "              ИТОГИ \n";
    cout << "========================================\n\n";
    cout << "Правильных ответов: " << score << " из " << total << "\n";
    cout << "Процент: " << (score * 100 / total) << "%\n\n";

    // Score evaluation
    if (score == total) {
        cout << "ИДЕАЛЬНО! Вы настоящий знаток!\n";
        cout << "ЗОЛОТАЯ МЕДАЛЬ!\n";
    }
    else if (score >= 4) {
        cout << "ВЕЛИКОЛЕПНО! Отличный результат!\n";
        cout << "СЕРЕБРЯНАЯ МЕДАЛЬ!\n";
    }
    else if (score >= 3) {
        cout << "ХОРОШО! Неплохой результат!\n";
        cout << "БРОНЗОВАЯ МЕДАЛЬ!\n";
    }
    else if (score >= 2) {
        cout << "УДОВЛЕТВОРИТЕЛЬНО! Есть куда расти!\n";
    }
    else {
        cout << "В следующий раз обязательно получится! Не сдавайтесь!\n";
    }

    cout << "\nСпасибо за игру!\n";
    cout << "\n";

    waitForEnter();

    return 0;
}