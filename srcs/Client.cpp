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

    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(_port);
    if (inet_pton(AF_INET, _serverIP, &serverAddr.sin_addr) <= 0) {
        perror("inet_pton");
        return false;
    }

    if (connect(_socket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("connect");
        return false;
    }

    std::cout << "Connected to server" << std::endl;
    return true;
}

bool Client::sendMessage(const std::string& message) {
    std::string formattedMessage = message + "\r\n";
    ssize_t sent = send(_socket, formattedMessage.c_str(), formattedMessage.length(), 0);
    if (sent == -1) {
        perror("send");
        return false;
    }

    std::cout << "Message sent to server: " << message << std::endl;
    return true;
}

bool Client::receiveMessage(std::string& receivedMessage) {
    char buffer[1024];
    std::string tempMessage;
    while (true) {
        ssize_t bytesRead = recv(_socket, buffer, sizeof(buffer) - 1, 0);
        if (bytesRead <= 0) {
            if (bytesRead == 0) {
                std::cout << "Server disconnected" << std::endl;
            } else {
                perror("recv");
            }
            return false;
        }

        buffer[bytesRead] = '\0';
        tempMessage += buffer;

        size_t pos = tempMessage.find("\r\n");
        if (pos != std::string::npos) {
            receivedMessage = tempMessage.substr(0, pos);
            tempMessage.erase(0, pos + 2);
            break;
        }
    }
    std::cout << "Received message from server: " << receivedMessage << std::endl;
    return true;
}

void Client::disconnect() {
    if (_socket > 0) {
        close(_socket);
        _socket = 0;
    }
    std::cout << "Disconnected from server" << std::endl;
}