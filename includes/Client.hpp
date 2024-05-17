#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <arpa/inet.h>
#include <iostream>

class Client {
public:
    Client(const char* serverIP, int port);
    ~Client();

    bool connectToServer();
    bool sendMessage(const std::string& message);
    bool receiveMessage(std::string& receivedMessage);
    void disconnect();

private:
    const char* _serverIP;
    int _port;
    int _socket;
};

#endif