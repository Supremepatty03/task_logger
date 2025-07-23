#include "Log_manager.hpp"
#include <fstream>
#include <string>
#include <chrono>
#include <iomanip>

using namespace std;

LogManager::LogManager(const string& destination, LogLevel lvl)
    : currentLevel(lvl)
{
    if (destination.rfind("tcp://", 0) == 0) {
        // tcp://127.0.0.1:12345
        auto colonPos = destination.find(':', 6);
        string ip   = destination.substr(6, colonPos - 6);
        int port         = stoi(destination.substr(colonPos + 1));
        logWriter        = make_unique<SocketLogWriter>(ip, port);
    } else {
        logWriter = make_unique<FileLogWriter>(destination);
    }

    if (logWriter && logWriter->isOpen()) {
        initialized = true;
    }
}
LogManager::~LogManager() {
    if (logFile.is_open()) {
        logFile.close();
    }
}
void LogManager::log(const string& message, LogLevel lvl) {
    
    if (!initialized || lvl < currentLevel) {
        return;
    }

    auto now = chrono::system_clock::now();
    auto tt  = chrono::system_clock::to_time_t(now);
    tm tm;
    localtime_r(&tt, &tm);

    ostringstream ts;
    ts << put_time(&tm, "%Y-%m-%d %H:%M:%S");

    // Формирование итоговой строки лога
    ostringstream entry;
    entry << ts.str()
          << " [" << getLevelString(lvl) << "] "
          << message
          << "\n";

    lock_guard<mutex> lock(mtx);

    // файл или сокет через интерфейс ILogWriter
    if (logWriter) {
        logWriter->write(entry.str());
    }
}

void LogManager::setLogLevel(LogLevel lvl) {
    currentLevel = lvl;
}
string LogManager::getTimestamp() {
    auto now = chrono::system_clock::now();
    time_t now_c = chrono::system_clock::to_time_t(now);
    stringstream ss;
    ss << put_time(localtime(&now_c), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

string LogManager::getLevelString(LogLevel lvl) const {
    switch (lvl) {
        case LogLevel::INFO: return "INFO";
        case LogLevel::WARNING: return "WARNING";
        case LogLevel::ERR: return "ERROR";
        default: return "UNKNOWN";
    }
}