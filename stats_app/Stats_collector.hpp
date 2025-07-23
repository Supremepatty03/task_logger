#pragma once
#include <string>
#include <map>
#include <chrono>
#include <mutex>
#include <vector>
#include "Log_manager.hpp"

using namespace std;

//enum class LogLevel { INFO, WARNING, ERR };

struct LogEntry {
    chrono::system_clock::time_point timestamp;
    string message;
    LogLevel level;
};

class LogStatsCollector {
public:
    void addMessage(const string& rawMessage);

    void printStats();

    bool shouldReport(int N, int T);
    void markReported();
    int totalCount() const { return total; }

private:
    mutex mtx;
    vector<LogEntry> messages;

    int total = 0;
    map<LogLevel, int> levelCount;

    size_t minLength = SIZE_MAX;
    size_t maxLength = 0;
    size_t totalLength = 0;

    chrono::system_clock::time_point lastReportTime;
    int lastReportedCount = 0;
    bool statsChanged = false;

    LogLevel parseLevel(const string& message);
    void cleanupOldEntries(); // очистка старше 1 часа
};