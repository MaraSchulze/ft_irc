#include "Client.hpp"

Client::Client(const char* serverIP, int port) : _serverIP(serverIP), _port(port), _socket(0) {}

Client::~Client() {
    disconnect();
}

bool Client::connectToServer() {
    _socket = socket(AF_INET, SOCK_STREAM, 0);
    if (_socket == -1) {
        perror("socket");
        return false;
    }

    int flags = fcntl(_socket, F_GETFL, 0);
    fcntl(_socket, F_SETFL, flags | O_NONBLOCK);

    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(_port);
    if (inet_pton(AF_INET, _serverIP, &serverAddr.sin_addr) <= 0) {
        perror("inet_pton");
        return false;
    }

    if (connect(_socket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        if (errno != EINPROGRESS) {
            perror("connect");
            return false;
        }
    }

    std::cout << "Connected to server" << std::endl;
    return true;
}

bool Client::sendMessage(const std::string& message) {
    std::string formattedMessage = message + "\r\n";
    ssize_t totalSent = 0;
    ssize_t messageLength = formattedMessage.length();

    while (totalSent < messageLength) {
        ssize_t sent = send(_socket, formattedMessage.c_str() + totalSent, messageLength - totalSent, 0);
        if (sent == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                continue; // Try again
            } else {
                perror("send");
                return false;
            }
        }
        totalSent += sent;
    }

    std::cout << "Message sent to server: " << message << std::endl;
    return true;
}

bool Client::receiveMessage(std::string& receivedMessage) {
    char buffer[1024];
    while (true) {
        ssize_t bytesRead = recv(_socket, buffer, sizeof(buffer) - 1, 0);
        if (bytesRead <= 0) {
            if (bytesRead == 0) {
                std::cout << "Server disconnected" << std::endl;
                return false;
            } else if (errno == EAGAIN || errno == EWOULDBLOCK) {
                break; // No more data to read
            } else {
                perror("recv");
                return false;
            }
        }

        buffer[bytesRead] = '\0';
        _recvBuffer += buffer;

        size_t pos;
        while ((pos = _recvBuffer.find("\r\n")) != std::string::npos) {
            receivedMessage = _recvBuffer.substr(0, pos);
            _recvBuffer.erase(0, pos + 2);
            std::cout << "Received message from server: " << receivedMessage << std::endl;
            return true;
        }
    }
    return false;
}

void Client::disconnect() {
    if (_socket > 0) {
        close(_socket);
        _socket = 0;
    }
    std::cout << "Disconnected from server" << std::endl;
}