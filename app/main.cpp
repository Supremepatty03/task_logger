#include "Log_manager.hpp"
#include <iostream>

using namespace std;

int main() {
    LogManager logger("log.txt", LogLevel::WARNING);

    if (!logger.isInitialized()) {
        cout << "Не удалось открыть лог-файл!" << std::endl;
        return 1;
    }

    logger.log("INFO message", LogLevel::INFO);      // будет проигнорировано
    logger.log("WARNING message", LogLevel::WARNING);
    logger.log("ERROR message", LogLevel::ERR);

    logger.setLogLevel(LogLevel::INFO);
    logger.log("INFO after resetting level", LogLevel::INFO);

    return 0;
}