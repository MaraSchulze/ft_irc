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
#include <cstdio>
#include <cerrno>
#include <map>
#include "SendQueue.hpp"
#include "User.hpp"
#include "IrcApplicationLayer.hpp"

class IrcApplicationLayer;

class Server {
public:
    Server(int port, IrcApplicationLayer& ircApp);
    ~Server();

    bool startListening();
    void run();
    bool acceptClient();
    bool receiveMessage(int clientSocket);
    bool sendMessage(int clientSocket);
    void disconnect();
	void disconnectClient(int id);

private:
    int _port;
    int _listener;
    std::vector<pollfd> _clients;
    std::map<int, std::string> _recvBuffers; // Buffers for accumulating received data per client
    std::map<int, std::string> _sendBuffers;
	IrcApplicationLayer& _ircApp;
	SendQueue& _sendQueue;
};

#endif 