#include "Socket_receiver.hpp"
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
using namespace std;


SocketReceiver::SocketReceiver(int socket_fd)
    : sockfd(socket_fd), running(true), readerThread(&SocketReceiver::readLoop, this) {}

SocketReceiver::~SocketReceiver() {
    stop();
}

void SocketReceiver::stop() {
    if (running) {
        running = false;
        shutdown(sockfd, SHUT_RD);
        if (readerThread.joinable())
            readerThread.join();
        close(sockfd);
    }
}

bool SocketReceiver::getNextMessage(string& message) {
    unique_lock<mutex> lock(mtx);
    queueCond.wait(lock, [this]() { return !messageQueue.empty() || !running; });

    if (!messageQueue.empty()) {
        message = move(messageQueue.front());
        messageQueue.pop();
        return true;
    }
    return false;
}

void SocketReceiver::readLoop() {
    char buffer[4096];

    while (running) {
        ssize_t bytesRead = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
        if (bytesRead > 0) {
            buffer[bytesRead] = '\0';
            string msg(buffer);

            {
                lock_guard<mutex> lock(mtx);
                messageQueue.push(msg);
            }
            queueCond.notify_one();
        } else {
            if (bytesRead == 0) {
                cout << "Сервер закрыл соединение\n";
            } else {
                cerr << "Ошибка чтения из сокета\n";
            }
            running = false;
        }
    }
}