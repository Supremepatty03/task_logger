#include "Stats_collector.hpp"
#include <iostream>
#include <sstream>
#include <regex>
#include <iomanip>

using namespace std;
using namespace std::chrono;

LogLevel LogStatsCollector::parseLevel(const string& msg) {
    smatch match;
    if (regex_search(msg, match, regex("\\[(INFO|WARNING|ERROR)\\]"))) {
        string lvl = match[1];
        if (lvl == "INFO") return LogLevel::INFO;
        if (lvl == "WARNING") return LogLevel::WARNING;
        if (lvl == "ERROR") return LogLevel::ERR;
    }
    return LogLevel::INFO; // по умолчанию
}

void LogStatsCollector::addMessage(const string& rawMessage) {
    lock_guard<mutex> lock(mtx);

    LogLevel lvl = parseLevel(rawMessage);
    auto now = system_clock::now();

    LogEntry entry{now, rawMessage, lvl};
    messages.push_back(entry);

    total++;
    levelCount[lvl]++;

    size_t len = rawMessage.size();
    totalLength += len;
    if (len < minLength) minLength = len;
    if (len > maxLength) maxLength = len;

    statsChanged = true;
    cleanupOldEntries();
}

void LogStatsCollector::cleanupOldEntries() {
    auto now = system_clock::now();
    auto cutoff = now - hours(1);

    messages.erase(remove_if(messages.begin(), messages.end(),
        [cutoff](const LogEntry& entry) {
            return entry.timestamp < cutoff;
        }), messages.end());
}

void LogStatsCollector::printStats() {
    lock_guard<mutex> lock(mtx);

    cout << "\n=== Статистика логов ===" << endl;
    cout << "Всего сообщений: " << totalCount() << endl;
    cout << "По уровням:" << endl;
    cout << "  INFO: " << levelCount[LogLevel::INFO] << endl;
    cout << "  WARNING: " << levelCount[LogLevel::WARNING] << endl;
    cout << "  ERROR: " << levelCount[LogLevel::ERR] << endl;

    int countLastHour = 0;
    auto cutoff = system_clock::now() - hours(1);
    for (const auto& entry : messages) {
        if (entry.timestamp >= cutoff) {
            countLastHour++;
        }
    }
    cout << "Сообщений за последний час: " << countLastHour << endl;

    if (totalCount() > 0) {
        double avg = static_cast<double>(totalLength) / totalCount();
        cout << "Длина сообщений:" << endl;
        cout << "  Мин.: " << minLength << " символов" << endl;
        cout << "  Макс.: " << maxLength << " символов" << endl;
        cout << "  Средняя: " << fixed << setprecision(2) << avg << " символов" << endl;
    }

    cout << "========================\n" << endl;
}

bool LogStatsCollector::shouldReport(int N, int T) {
    lock_guard<mutex> lock(mtx);
    auto now = system_clock::now();

    bool timeElapsed = duration_cast<seconds>(now - lastReportTime).count() >= T;
    bool countReached = (totalCount() - lastReportedCount) >= N;

    return statsChanged && (countReached || timeElapsed);
}

void LogStatsCollector::markReported() {
    lock_guard<mutex> lock(mtx);
    lastReportedCount = totalCount();
    lastReportTime = system_clock::now();
    statsChanged = false;
}