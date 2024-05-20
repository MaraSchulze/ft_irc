#include "Server.hpp"

Server::Server(int port, IrcApplicationLayer& ircApp) : _port(port), _listener(0), _ircApp(ircApp), _sendQueue(_ircApp.getSendQueue()) {
	_ircApp.registerServer(this);
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

    std::cout << "[socket layer] Server listening on port " << _port << std::endl;
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
    _sendBuffers[clientSocket] = "";

    std::cout << "[socket layer] Client connected" << std::endl;
    return true;
}

bool Server::receiveMessage(int clientSocket) {
    char buffer[1024];

	ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
	if (bytesRead <= 0) {
		if (bytesRead == 0) {
			std::cout << "[socket layer] Client disconnected" << std::endl;
			return false;
		} else if (errno == EAGAIN || errno == EWOULDBLOCK) {
			return true; // No more data to read
		} else {
			perror("recv");
			return false;
		}
	}

	buffer[bytesRead] = '\0';
	_recvBuffers[clientSocket] += buffer;

	size_t pos;
	while ((pos = _recvBuffers[clientSocket].find("\r\n")) != std::string::npos) {
		std::string message = _recvBuffers[clientSocket].substr(0, pos);
		_recvBuffers[clientSocket].erase(0, pos + 2);
		std::cout << "[socket layer] Received complete message from client: " << message << std::endl;
		_ircApp.receive(clientSocket, message);
	}
    return true;
}

bool Server::sendMessage(int clientSocket) {
	if (!_sendBuffers[clientSocket].empty()) {
		ssize_t nbytes = send(clientSocket, _sendBuffers[clientSocket].c_str(), _sendBuffers[clientSocket].size(), 0);
		if (nbytes > 0) {
			_sendBuffers[clientSocket].erase(0, nbytes);
			for (size_t i = 0; i < _clients.size(); ++i) {
				if (!_sendBuffers[_clients[i].fd].empty())
					_clients[i].events &= ~POLLOUT;
			}
			return true;
		} else if (nbytes == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
			return true;
		} else {	
			perror("send");
			return false;
		}
	}
	return true;
}

void Server::disconnect() {
    for (size_t i = 0; i < _clients.size(); ++i) {
        close(_clients[i].fd);
    }
    _clients.clear();
    std::cout << "[socket layer] Server disconnected" << std::endl;
}

/*
This function is being called from the ircApplicationLayer object after a QUIT.
We clean up our data structures and mark the file descriptor as -1, so that
we can delete it from _client later.
*/
void Server::disconnectClient(int clientFd) {
    close(clientFd);
    _recvBuffers.erase(clientFd);
	_sendBuffers.erase(clientFd);
    for (size_t i = 0; i < _clients.size(); ++i) {
    	if (_clients[i].fd == clientFd) {
			_clients[i].fd = -1;
		}
	}
}

void Server::run() {
	int	clientSocket;
    std::string message;
	int clientFd;

	startListening();

    while (true) {
        int pollCount = poll(&_clients[0], _clients.size(), -1);
        if (pollCount == -1) {
            perror("poll");
            break;
        }

        for (size_t i = 0; i < _clients.size(); ++i) {
			clientFd = _clients[i].fd;

            if (_clients[i].revents & POLLIN) {
                if (clientFd == _listener) {
                    acceptClient();
                } else {
                    if (!receiveMessage(clientFd)) {
						_ircApp.disconnect(clientFd);
                        close(clientFd);
                        _recvBuffers.erase(clientFd);
						_sendBuffers.erase(clientFd);
                        _clients.erase(_clients.begin() + i);
                        --i;
					}
                }
            }

			if (_clients[i].fd != -1 && (_clients[i].revents & POLLOUT) && _clients[i].fd != _listener) {
				clientFd = _clients[i].fd;
				if (!sendMessage(clientFd)) {
					_ircApp.disconnect(clientFd);
					close(clientFd);
					_recvBuffers.erase(clientFd);
					_sendBuffers.erase(clientFd);
					_clients.erase(_clients.begin() + i);
					--i;		
				}
			}
		}
		
		while (! _sendQueue.empty()) {
			clientSocket = _sendQueue.getNextSender();
			message = _sendQueue.getNextMessage();
			_sendQueue.pop();
			_sendBuffers[clientSocket] += message;
		}

        for (size_t i = 0; i < _clients.size(); ++i) {
			if (_clients[i].fd == -1) {
				_clients.erase(_clients.begin() + i);
				--i;
			}
			if (!_sendBuffers[_clients[i].fd].empty())
				_clients[i].events |= POLLOUT;
		}
    }

	disconnect();
}