#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "User.hpp"
# include "Channel.hpp"
# include "SendQueue.hpp"
# include <map>
# include <string>
# include <netinet/in.h>
# include <vector>

# define SERVERNAME "irc.rockingpiggies.com"

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <arpa/inet.h>

class Client {
public:
    Client(const char* serverIP, int port);
    ~Client();

    bool connectToServer();
    bool sendMessage(const std::string& message);
    void disconnect();

private:
    const char* _serverIP;
    int _port;
    int _socket;
};

#endif
