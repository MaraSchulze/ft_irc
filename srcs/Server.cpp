#include "Server.hpp"

Server::Server(int port) : _port(port), _listener(0), _clientSocket(0) {}

Server::~Server() {
    disconnect();
}

bool Server::startListening() {
    // Create socket
    _listener = socket(AF_INET, SOCK_STREAM, 0);
    if (_listener == -1) {
        perror("socket");
        return false;
    }

    // Server address
    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(_port);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    // Bind
    if (bind(_listener, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("bind");
        return false;
    }

    // Listen
    if (listen(_listener, 10) == -1) {
        perror("listen");
        return false;
    }

    std::cout << "Server listening on port " << _port << std::endl;
    return true;
}

bool Server::acceptClient() {
    // Accept connection from client
    struct sockaddr_in clientAddr;
    socklen_t clientLen = sizeof(clientAddr);
    _clientSocket = accept(_listener, (struct sockaddr*)&clientAddr, &clientLen);
    if (_clientSocket == -1) {
        perror("accept");
        return false;
    }

    std::cout << "Client connected" << std::endl;
    return true;
}

bool Server::receiveMessage(std::string& receivedMessage) {
    // Receive message from client
    char buffer[1024];
    ssize_t bytesRead = recv(_clientSocket, buffer, sizeof(buffer) - 1, 0);
    if (bytesRead <= 0) {
        if (bytesRead == 0) {
            std::cout << "Client disconnected" << std::endl;
        } else {
            perror("recv");
        }
        return false;
    }

    buffer[bytesRead] = '\0';
    receivedMessage = buffer;
    std::cout << "Received message from client: " << receivedMessage << std::endl;
    return true;
}

void Server::disconnect() {
    // Close client socket
    if (_clientSocket > 0) {
        close(_clientSocket);
        _clientSocket = 0;
    }
    // Close listener socket
    if (_listener > 0) {
        close(_listener);
        _listener = 0;
    }
    std::cout << "Server disconnected" << std::endl;
}