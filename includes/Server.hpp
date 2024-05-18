#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <poll.h>
#include <fcntl.h>
#include <map>
#include "SendQueue.hpp"
#include "User.hpp"
#include "IrcApplicationLayer.hpp"

class Server {
public:
    Server(int port, SendQueue& sendQueue);
    ~Server();

    bool startListening();
    void run();
    bool acceptClient();
    bool receiveMessage(int clientSocket, std::string& receivedMessage);
    bool sendMessage(int clientSocket, const std::string& message);
    void disconnect();

private:
    int _port;
    int _listener;
    std::vector<pollfd> _clients;
    SendQueue& _sendQueue;
    std::map<int, std::string> _recvBuffers; // Buffers for accumulating received data per client
    std::vector<User> _users;
};

#endif 