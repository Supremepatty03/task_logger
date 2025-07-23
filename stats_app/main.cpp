#include "Log_manager.hpp"
#include "Socket_receiver.hpp"
#include "Stats_collector.hpp"
#include <iostream>
#include <queue>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring> 

using namespace std;

int main (int argc, char* argv[]) {
    if (argc != 4) {
        cerr << "Использование: " << argv[0] << " <port> <N (количество сообщений до отчета)> <T (таймер отчета)>" << endl;
        return 1;
    };

    // Порт 
    char* endptr = nullptr;
    long port_l = strtol(argv[1], &endptr, 10);
    if (*endptr != '\0' || port_l <= 0 || port_l > 65535) {
        cerr << "Неверный порт: " << argv[1] << "\n";
        return 1;
    }
    int port = static_cast<int>(port_l);

    // N 
    endptr = nullptr;
    long n_l = strtol(argv[2], &endptr, 10);
    if (*endptr != '\0' || n_l <= 0) {
        cerr << "Неверное значение N: " << argv[2] << "\n";
        return 1;
    }
    int N = static_cast<int>(n_l);

    // T 
    endptr = nullptr;
    long t_l = strtol(argv[3], &endptr, 10);
    if (*endptr != '\0' || t_l <= 0) {
        cerr << "Неверное значение T: " << argv[3] << "\n";
        return 1;
    }
    int T = static_cast<int>(t_l);

    // Проверка
    cout << "Сервер статистики на порту " << port << ", отчёт каждые " << N << " сообщений или " << T << " сек\n";

    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (listen_fd < 0) 
    {
        cerr << "Не удалось создать серверный сокет\n";
        return 1;
    }   

    int opt = 1;
    if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        cerr << "Ошибка setsockopt(SO_REUSEADDR)\n";
        close(listen_fd);
        return 1;
    }
    sockaddr_in srv{};
    srv.sin_family      = AF_INET;
    srv.sin_addr.s_addr = INADDR_ANY;
    srv.sin_port        = htons(port);

    if (bind(listen_fd, (struct sockaddr*)&srv, sizeof(srv)) < 0) {
        cerr << "Не удалось bind() на порт " << port << "\n";
        close(listen_fd);
        return 1;
    }

    if (listen(listen_fd, 1) < 0) {
        cerr << "Не удалось listen() на порт " << port << "\n";
        close(listen_fd);
        return 1;
    }   

    cout << "Ожидаю подключений...\n";
    int client_fd = accept(listen_fd, nullptr, nullptr);
    cout << "Клиент подключился.\n";

    SocketReceiver receiver(client_fd);
    LogStatsCollector stats;

    queue<string> msgQueue;
    mutex mtx;
    condition_variable cv;
    atomic<bool> running{true};

    // Поток для чтения сообщений из сокета
    thread readerThread([&]() {
        string message;
        while (running && receiver.getNextMessage(message)) {
            lock_guard<mutex> lock(mtx);
            msgQueue.push(message);
            cv.notify_one();
        }
        running = false;
        cv.notify_all();
    });

    auto lastReportTime = chrono::steady_clock::now();

    // Основной цикл обработки сообщений
    while (running) {
        unique_lock<mutex> lock(mtx);
        cv.wait_for(lock, chrono::seconds(1),
                    [&]() { return !msgQueue.empty() || !running; });

        bool updated = false;
        while (!msgQueue.empty()) {
            string msg = msgQueue.front();
            msgQueue.pop();
            lock.unlock();

            cout << "Получено: " << msg << "\n";
            stats.addMessage(msg);
            updated = true;

            lock.lock();
        }

        auto now = chrono::steady_clock::now();
        auto elapsed = chrono::duration_cast<chrono::seconds>(now - lastReportTime).count();

        if ((stats.totalCount() % N == 0 && stats.totalCount() > 0) ||
            (elapsed >= T && updated)) {
            stats.printStats();
            lastReportTime = now;
        }
    }

    readerThread.join();
    close(client_fd);
    close(listen_fd);
    return 0;
}
    