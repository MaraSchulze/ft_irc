#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <arpa/inet.h>

class SocketLayer {
public:
    SocketLayer();
    ~SocketLayer();

    bool connectToServer(const std::string& serverIP, int port);
    bool startServer(int port);
    bool acceptClient();
    bool sendMessage(const std::string& message);
    bool receiveMessage(std::string& receivedMessage);
    void disconnect();

private:
    int _socket;
    int _clientSocket; 
    struct sockaddr_in _address;
};

#endif 