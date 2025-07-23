#pragma once
#include <fstream>
#include <string>
#include <mutex>
#include "Log_writer.hpp"
#include <memory>


using namespace std;

enum class LogLevel {
    INFO,
    WARNING,
    ERR
};

class LogManager {
public: 
    LogManager(const string& filename, LogLevel lvl) ;
    ~LogManager();

    void log(const string& message, LogLevel lvl);
    void setLogLevel(LogLevel lvl);
    bool isInitialized() const { return initialized; }

private:
    ofstream logFile;
    LogLevel currentLevel;
    string getLevelString(LogLevel lvl) const;
    string getTimestamp();
    bool initialized = false;
    mutex mtx;

    unique_ptr <ILogWriter> logWriter; 
};