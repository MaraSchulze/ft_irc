#include "socket.hpp"
#include <iostream>

SocketLayer::SocketLayer() : _socket(0), _clientSocket(0) {
    memset(&_address, 0, sizeof(_address));
}

SocketLayer::~SocketLayer() {
    disconnect();
}

bool SocketLayer::connectToServer(const std::string& serverIP, int port) {
    _socket = socket(AF_INET, SOCK_STREAM, 0);
    if (_socket == -1) {
        perror("socket");
        return false;
    }

    _address.sin_family = AF_INET;
    _address.sin_port = htons(port);

    if (inet_pton(AF_INET, serverIP.c_str(), &_address.sin_addr) <= 0) {
        perror("inet_pton");
        return false;
    }

    if (connect(_socket, (struct sockaddr*)&_address, sizeof(_address)) == -1) {
        perror("connect");
        return false;
    }

    std::cout << "Connected to server" << std::endl;
    return true;
}

bool SocketLayer::startServer(int port) {
    _socket = socket(AF_INET, SOCK_STREAM, 0);
    if (_socket == -1) {
        perror("socket");
        return false;
    }

    _address.sin_family = AF_INET;
    _address.sin_port = htons(port);
    _address.sin_addr.s_addr = INADDR_ANY;

    if (bind(_socket, (struct sockaddr*)&_address, sizeof(_address)) == -1) {
        perror("bind");
        return false;
    }

    if (listen(_socket, 10) == -1) {
        perror("listen");
        return false;
    }

    std::cout << "Server started, listening on port " << port << std::endl;
    return true;
}

bool SocketLayer::acceptClient() {
    socklen_t addrLen = sizeof(_address);
    _clientSocket = accept(_socket, (struct sockaddr*)&_address, &addrLen);
    if (_clientSocket == -1) {
        perror("accept");
        return false;
    }

    std::cout << "Client connected" << std::endl;
    return true;
}

bool SocketLayer::sendMessage(const std::string& message) {
    int socket = (_clientSocket > 0) ? _clientSocket : _socket;
    if (send(socket, message.c_str(), message.length(), 0) == -1) {
        perror("send");
        return false;
    }
    return true;
}

bool SocketLayer::receiveMessage(std::string& receivedMessage) {
    char buffer[1024];
    int socket = (_clientSocket > 0) ? _clientSocket : _socket;
    ssize_t bytesRead = recv(socket, buffer, sizeof(buffer) - 1, 0);
    if (bytesRead <= 0) {
        if (bytesRead == 0) {
            std::cout << "Connection closed" << std::endl;
        } else {
            perror("recv");
        }
        return false;
    }

    buffer[bytesRead] = '\0';
    receivedMessage = buffer;
    return true;
}

void SocketLayer::disconnect() {
    if (_clientSocket > 0) {
        close(_clientSocket);
        _clientSocket = 0;
    }
    if (_socket > 0) {
        close(_socket);
        _socket = 0;
    }
    std::cout << "Disconnected" << std::endl;
}