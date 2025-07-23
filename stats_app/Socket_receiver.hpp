#pragma once
#include <string>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <atomic>

using namespace std;

class SocketReceiver {
public:
    SocketReceiver(int socket_fd);
    ~SocketReceiver();

    bool getNextMessage(string& message);

    void stop();

private:
    void readLoop();

    int sockfd;
    queue<string> messageQueue;
    mutex mtx;
    condition_variable queueCond;
    thread readerThread;
    atomic<bool> running;
};