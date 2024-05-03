
#include "Client.hpp"

Client::Client(const char* serverIP, int port) : _serverIP(serverIP), _port(port), _socket(0) {}

Client::~Client() {}

bool Client::connectToServer() {
    // Create socket
    _socket = socket(AF_INET, SOCK_STREAM, 0);
    if (_socket == -1) {
        perror("socket");
        return false;
    }

    // Server address
    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(_port);
    inet_pton(AF_INET, _serverIP, &serverAddr.sin_addr);

    // Connect to server
    if (connect(_socket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("connect");
        return false;
    }

    std::cout << "Connected to server" << std::endl;
    return true;
}

bool Client::sendMessage(const std::string& message) {
    // Send message to server
    ssize_t sent = send(_socket, message.c_str(), message.length(), 0);
    if (sent == -1) {
        perror("send");
        return false;
    }

    std::cout << "Message sent to server: " << message << std::endl;
    return true;
}

void Client::disconnect() {
    // Close socket
    close(_socket);
    std::cout << "Disconnected from server" << std::endl;
}


/*int main() {
    // Create client instance
    Client client("127.0.0.1", 6667);

    // Connect to server
    if (!client.connectToServer()) {
        std::cerr << "Failed to connect to server" << std::endl;
        return 1;
    }

    // Send message to server
    std::string message = "Hello, server!";
    if (!client.sendMessage(message)) {
        std::cerr << "Failed to send message to server" << std::endl;
        return 1;
    }

    // Disconnect from server
    client.disconnect();

    return 0;
}
*/