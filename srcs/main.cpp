/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marschul <marschul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/17 16:09:12 by marschul          #+#    #+#             */
/*   Updated: 2024/04/24 21:01:36 by marschul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "IrcApplicationLayer.hpp"
#include <iostream>

int main(int argc, char **argv) {
	if (argc != 3) {
		std::cout << "usage: ./ircserv <port> <password>" << std::endl;
		return 1;
	}

	IrcApplicationLayer ircApp = IrcApplicationLayer(argv[2]);

	// test
	struct sockaddr_in address = {};
	ircApp.connect(100, address);
	ircApp.receive(100, "PASS doh");
	ircApp.receive(100, "NICK nicky");
	ircApp.receive(100, "USER whoever 0 * :whoever");
	ircApp.receive(100, "JOIN #testchannel,#secondchannel key");
	ircApp.receive(100, "GARBLE");
	ircApp.receive(100, "TOPIC #testchannel :pigs can fly.");
	ircApp.connect(101, address);
	ircApp.receive(101, "PASS doh");
	ircApp.receive(101, "NICK wicky");
	ircApp.receive(101, "USER x 0 * :x");
	ircApp.receive(101, "JOIN #testchannel");
	ircApp.receive(100, "PRIVMSG #testchannel :Hello");
	ircApp.receive(100, "INVITE wicky #secondchannel");
	ircApp.receive(100, "KICK #secondchannel wicky,noone");
	ircApp.receive(100, "MODE #testchannel +k key");
	ircApp.disconnect(100);
}
