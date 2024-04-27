#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <fcntl.h>
#include <arpa/inet.h>
#include <string>
#include "IrcApplicationLayer.hpp"
#include "SendQueue.hpp"

int main() {
	IrcApplicationLayer ircApp = IrcApplicationLayer("doh");
	SendQueue& sendQueue = ircApp.getSendQueue();
    
	int listener = socket(AF_INET, SOCK_STREAM, 0);
    if (listener == -1) {
        perror("socket");
        return 1;
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(6667);
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (bind(listener, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        perror("bind");
        return 2;
    }

    listen(listener, 10);

    fd_set master;    // Master file descriptor list
    fd_set read_fds;  // Temp file descriptor list for select()
    int fdmax;        // Maximum file descriptor number

    FD_ZERO(&master);    // Clear the master and temp sets
    FD_ZERO(&read_fds);

    // Add the listener to the master set
    FD_SET(listener, &master);

    // Keep track of the biggest file descriptor
    fdmax = listener; // So far, it's this one

    while (true) {
        read_fds = master; // Copy it
        if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) {
            perror("select");
            exit(4);
        }

        // Run through the existing connections looking for data to read
        for(int i = 0; i <= fdmax; i++) {
            if (FD_ISSET(i, &read_fds)) { // we got one!!
                if (i == listener) {
                    // Handle new connections
                    struct sockaddr_in client_addr;
                    socklen_t addrlen = sizeof(client_addr);
                    int newfd = accept(listener, (struct sockaddr *)&client_addr, &addrlen);
                    if (newfd < 0) {
                        perror("accept");
                    } else {
                        FD_SET(newfd, &master); // Add to master set
                        if (newfd > fdmax) {    // Keep track of the max
                            fdmax = newfd;
                        }
						ircApp.connect(newfd, client_addr);

                    }
                } else {
                    // Handle data from a client
                    char buf[1024];
                    int nbytes = recv(i, buf, sizeof(buf), 0);
                    if (nbytes <= 0) {
                        // Got error or connection closed by client
                        if (nbytes == 0) {
                            // Connection closed
                            std::cout << "socket " << i << " hung up" << std::endl;
							ircApp.disconnect(i);
                        } else {
                            perror("recv");
                        }
                        close(i); // Bye!
                        FD_CLR(i, &master); // Remove from master set
                    } else {
						buf[nbytes] = '\0';
                        ircApp.receive(i, buf);
                    }
                } 
            }
        }

		// send to clients
		int clientSocket;
		std::string	sendMessage;
		while (sendQueue.empty() == false) {
			clientSocket = sendQueue.getNextSender();
			sendMessage = sendQueue.getNextMessage();
			sendQueue.pop();
			int sent = send(clientSocket, sendMessage.c_str(), sendMessage.size(), 0);
			if (sent < 0) {
				perror("send");
				break;
			}
		}
    }

    return 0;
}
