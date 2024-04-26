#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <string>
#include "IrcApplicationLayer.hpp"
#include "SendQueue.hpp"

int main() {
    int listener = socket(AF_INET, SOCK_STREAM, 0);
    if (listener < 0) {
        perror("socket");
        exit(1);
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(6667); // Port number
    addr.sin_addr.s_addr = htonl(INADDR_ANY); // Listen on any network interface

    if (bind(listener, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        exit(2);
    }

    listen(listener, 1);

    struct sockaddr_in client_addr;
	socklen_t client_len = sizeof(client_addr);
	int sock = accept(listener, (struct sockaddr*)&client_addr, &client_len);
	if (sock < 0) {
		perror("accept");
		exit(3);
	}

    char buf[1024];
	IrcApplicationLayer ircApp = IrcApplicationLayer("doh");
	SendQueue& sendQueue = ircApp.getSendQueue();
	ircApp.connect(sock, client_addr);

    while (true) {
        // Receive
        int received = recv(sock, buf, 1024, 0);
        if (received < 0) {
            perror("recv");
            break;
        }
		if (received == 0)
			continue;

		// process
		buf[received] = '\0';
		std::string message = buf;
		ircApp.receive(sock, message);

        // Send message
		int clientSocket;
		std::string	sendMessage;
		while (sendQueue.empty() == false) {
			clientSocket = sendQueue.getNextSender();
			sendMessage = sendQueue.getNextMessage();
			sendQueue.pop();
			if (clientSocket == sock) {
				int sent = send(sock, sendMessage.c_str(), sendMessage.size(), 0);
				if (sent < 0) {
					perror("send");
					break;
				}
			}
		}
    }

    close(sock);
    return 0;
}
