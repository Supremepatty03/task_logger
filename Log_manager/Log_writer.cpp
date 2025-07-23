#include "Log_writer.hpp"
#include <string>
#include <fstream>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;

SocketLogWriter::SocketLogWriter(const std::string& ip, int port) {
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        connected = false;
        return;
    }

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    
    if (inet_pton(AF_INET, ip.c_str(), &server_addr.sin_addr) <= 0) {
        connected = false;
        return;
    }

    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == 0) {
        connected = true;
    } else {
        connected = false;
        close(sock);
        sock = -1;
    }
}
void SocketLogWriter::write(const std::string& message) {
    if (connected) {
        send(sock, message.c_str(), message.length(), 0);
    }
}

bool SocketLogWriter::isOpen() const {
    return connected;
}

SocketLogWriter::~SocketLogWriter() {
    if (sock >= 0) close(sock);
}