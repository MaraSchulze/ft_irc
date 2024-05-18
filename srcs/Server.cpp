#include "Server.hpp"

Server::Server(int port, IrcApplicationLayer& ircApp) : _port(port), _listener(0), _ircApp(ircApp), _sendQueue(_ircApp.getSendQueue()) {

}

Server::~Server() {
    disconnect();
}

bool Server::startListening() {
    _listener = socket(AF_INET, SOCK_STREAM, 0);
    if (_listener == -1) {
        perror("socket");
        return false;
    }

    int flags = fcntl(_listener, F_GETFL, 0);
    fcntl(_listener, F_SETFL, flags | O_NONBLOCK);

    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(_port);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(_listener, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("bind");
        return false;
    }

    if (listen(_listener, 10) == -1) {
        perror("listen");
        return false;
    }

    struct pollfd pfd;
    pfd.fd = _listener;
    pfd.events = POLLIN;
    _clients.push_back(pfd);

    std::cout << "Server listening on port " << _port << std::endl;
    return true;
}

bool Server::acceptClient() {
    struct sockaddr_in clientAddr;
    socklen_t clientLen = sizeof(clientAddr);
    int clientSocket = accept(_listener, (struct sockaddr*)&clientAddr, &clientLen);
    if (clientSocket == -1) {
        perror("accept");
        return false;
    }

    int flags = fcntl(clientSocket, F_GETFL, 0);
    fcntl(clientSocket, F_SETFL, flags | O_NONBLOCK);

    struct pollfd pfd;
    pfd.fd = clientSocket;
    pfd.events = POLLIN;
    _clients.push_back(pfd);

	_ircApp.connect(clientSocket, clientAddr);

    _recvBuffers[clientSocket] = "";

    std::cout << "Client connected" << std::endl;
    return true;
}

bool Server::receiveMessage(int clientSocket, std::string& receivedMessage) {
    char buffer[1024];
    while (true) {
        ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytesRead <= 0) {
            if (bytesRead == 0) {
                std::cout << "Client disconnected" << std::endl;
                return false;
            } else if (errno == EAGAIN || errno == EWOULDBLOCK) {
                break; // No more data to read
            } else {
                perror("recv");
                return false;
            }
        }

        buffer[bytesRead] = '\0';
        _recvBuffers[clientSocket] += buffer;

        size_t pos;
        while ((pos = _recvBuffers[clientSocket].find("\r\n")) != std::string::npos) {
            receivedMessage = _recvBuffers[clientSocket].substr(0, pos);
            _recvBuffers[clientSocket].erase(0, pos + 2);
            std::cout << "Received complete message from client: " << receivedMessage << std::endl;
            return true;
        }
    }
    return false;
}

bool Server::sendMessage(int clientSocket, const std::string& message) {
    std::string formattedMessage = message + "\r\n";
    ssize_t totalSent = 0;
    ssize_t messageLength = formattedMessage.length();

    while (totalSent < messageLength) {
        ssize_t sent = send(clientSocket, formattedMessage.c_str() + totalSent, messageLength - totalSent, 0);
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

    std::cout << "Message sent to client: " << message << std::endl;
    return true;
}

void Server::disconnect() {
    for (size_t i = 0; i < _clients.size(); ++i) {
        close(_clients[i].fd);
    }
    _clients.clear();
    std::cout << "Server disconnected" << std::endl;
}

void Server::run() {
	startListening();

    while (true) {
        int pollCount = poll(&_clients[0], _clients.size(), -1);
        if (pollCount == -1) {
            perror("poll");
            break;
        }

        for (size_t i = 0; i < _clients.size(); ++i) {
            if (_clients[i].revents & POLLIN) {
                if (_clients[i].fd == _listener) {
                    acceptClient();
                } else {
					int	clientSocket;
                    std::string message;
                    if (!receiveMessage(_clients[i].fd, message)) {
						_ircApp.disconnect(_clients[i].fd);
                        close(_clients[i].fd);
                        _recvBuffers.erase(_clients[i].fd);
                        _clients.erase(_clients.begin() + i);
                        --i;
                    } else {
						_ircApp.receive(_clients[i].fd, message);
                        while (!_sendQueue.empty()) {
							clientSocket = _sendQueue.getNextSender();
							message = _sendQueue.getNextMessage();
							_sendQueue.pop();
                            sendMessage(clientSocket, message);
                        }
                    }
                }
            }
        }
    }
}