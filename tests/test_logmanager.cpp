#include "../Log_manager/Log_manager.hpp"
#include <gtest/gtest.h>
#include <fstream>
#include <string>
#include <filesystem>
using namespace std;

TEST(LogManagerTest, LogFileCreation) {
    string logFilePath = "test_log.txt";
    LogManager logManager(logFilePath, LogLevel::INFO);

    // Создание файла
    EXPECT_TRUE(filesystem::exists(logFilePath));

    filesystem::remove(logFilePath);
}

TEST(LogManagerTest, LogMessage) {
    string logFilePath = "test_log.txt";
    LogManager logManager(logFilePath, LogLevel::INFO);

    logManager.log("This is an info message", LogLevel::INFO);

    // Проверка, что сообщение записано в файл
    ifstream logFile(logFilePath);
    string line;
    bool found = false;
    while (getline(logFile, line)) {
        if (line.find("This is an info message") != string::npos) {
            found = true;
            break;
        }
    }
    EXPECT_TRUE(found);

    // Clean up
    filesystem::remove(logFilePath);
}

TEST(LogManagerTest, LogLevelChange) {
    string logFilePath = "test_log.txt";
    LogManager logManager(logFilePath, LogLevel::INFO);

    logManager.log("This is an info message", LogLevel::INFO);
    
    // Смена уровня на WARNING
    logManager.setLogLevel(LogLevel::WARNING);
    
    logManager.log("This should not be logged", LogLevel::INFO);

    logManager.log("This is a warning message", LogLevel::WARNING);

    // отображение WARNING
    ifstream logFile(logFilePath);
    string line;
    bool foundWarning = false;
    while (getline(logFile, line)) {
        if (line.find("This is a warning message") != string::npos) {
            foundWarning = true;
            break;
        }
    }
    EXPECT_TRUE(foundWarning);

    // тест на INFO, который не должен быть записан
    logFile.clear();
    logFile.seekg(0);
    bool foundInfo = false;
    while (getline(logFile, line)) {
        if (line.find("This should not be logged") != string::npos) {
            foundInfo = true;
            break;
        }
    }
    EXPECT_FALSE(foundInfo);

    // Clean up
    filesystem::remove(logFilePath);
}