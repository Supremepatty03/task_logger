#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include "Log_manager.hpp"
#include <iostream>
#include <vector>
#include <thread>
#include <locale>
#include "File_manager.hpp"

using namespace std;

inline void logMessage(LogManager& logger, const string& message, LogLevel level) {
    logger.log(message, level);
}

// Преобразование строки в LogLevel
LogLevel parseLogLevel(const string& input, LogLevel defaultLevel = LogLevel::INFO) {
    if (input == "INFO") return LogLevel::INFO;
    if (input == "WARNING") return LogLevel::WARNING;
    if (input == "ERROR") return LogLevel::ERR;
    return defaultLevel;
}

int main(int argc, char* argv[]) {
    
    #ifdef _WIN32
        #include <windows.h> // Windows 
        SetConsoleOutputCP(CP_UTF8);
        SetConsoleCP(CP_UTF8);
    #else
        setlocale(LC_ALL, "ru_RU.UTF-8"); // Linux
    #endif
    string filename;
    LogLevel defaultLvl = LogLevel::INFO;

    if (argc < 2) {
        cerr << "Использование: " << argv[0] << " <Имя_файла / tcp сокет> [<Уровень по умолчанию>]\n";
        cerr << "Режим ручного ввода ..." << endl;
        
        while (true) 
        {
            cout << "Введите имя файла / сокет для логирования: ";
            getline(cin, filename);

            if (filename.rfind("tcp://", 0) == 0) { break; }  // Если начинается с "tcp://", то это сокет

            if (!FileManager::is_valid_filename(filename)) {
                cout << "Недопустимое имя файла. Попробуйте снова." << endl;
                continue;
            }

            if (FileManager::file_existance(filename)) {
                cout << "Файл уже существует. Перезаписать? (y/n): ";
                string choice;
                getline(cin, choice);
                if (choice != "y" && choice != "Y") {
                    cout << "Выберите другое имя." << endl;
                    continue;
                }
            }
            break;
        }
        cout << "Введите уровень логирования (INFO, WARNING, ERROR): ";
        string inputLevel; 
        getline(cin, inputLevel);

        defaultLvl = parseLogLevel(inputLevel, LogLevel::INFO);
    }
    else{
        filename = argv[1];

        if (argc >= 3) {
            defaultLvl = parseLogLevel(argv[2], LogLevel::INFO);
        }
    }
    LogManager logger(filename, defaultLvl);

    if (!logger.isInitialized()) {
        cerr << "Не удалось инициализировать логгер." << endl;
        return 1;
    }

    cout << "Уровни важности: INFO, WARNING, ERROR. Для выхода — пустая строка." << endl;
    cout << "Введите сообщения для логирования (формат: <сообщение> [<уровень>])." << endl;
    cout << "Пример: \"Сообщение об ошибке [ERROR]\"." << endl;

    vector<thread> threads;
    string input;

    // считывание сообщений
    while (true) {
        cout << "> ";
        getline(cin, input);

        if (input.empty()) break;

        stringstream ss(input);
        string messagePart, levelPart;
        getline(ss, messagePart, '[');
        getline(ss, levelPart, ']');

        string trimmedMessage = messagePart;
        string trimmedLevel = levelPart;

        // Удаление пробелов
        trimmedMessage.erase(trimmedMessage.find_last_not_of(" \t") + 1);
        trimmedLevel.erase(0, trimmedLevel.find_first_not_of(" \t"));
        trimmedLevel.erase(trimmedLevel.find_last_not_of(" \t") + 1);

        LogLevel lvl = parseLogLevel(trimmedLevel, LogLevel::INFO);

        if (lvl >= defaultLvl) 
        {
            threads.emplace_back(logMessage, ref(logger), trimmedMessage, lvl);
        }
    }

    for (auto& t : threads) {
        if (t.joinable()) t.join();
    }

    cout << "Завершено. Сообщения записаны в " << filename << endl;
    return 0;
}