#ifndef SERVER_HPP
# define SERVER_HPP

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

class Server {
public:
    Server(int port);
    ~Server();

    bool startListening();
    bool acceptClient();
    bool receiveMessage(std::string& receivedMessage);
    void disconnect();

private:
    int _port;
    int _listener;
    int _clientSocket;
};

#endif