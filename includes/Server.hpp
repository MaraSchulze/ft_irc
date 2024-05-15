#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <iostream>

class Server {
public:
    Server(int port);
    ~Server();

    bool startListening();
    bool acceptClient();
    bool receiveMessage(std::string& receivedMessage);
    bool sendMessage(const std::string& message);
    void disconnect();

private:
    int _port;
    int _listener;
    int _clientSocket;
    struct sockaddr_in _address;
};

#endif