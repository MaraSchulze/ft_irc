#include "IrcApplicationLayer.hpp"
#include <iostream>

void	registerTestusers(IrcApplicationLayer& ircApp) {
	struct sockaddr_in address = {};
	ircApp.connect(100, address);
	ircApp.receive(100, "PASS password");
	ircApp.receive(100, "NICK nicky");
	ircApp.receive(100, "USER x 0 * x");

	ircApp.connect(101, address);
	ircApp.receive(101, "PASS password");
	ircApp.receive(101, "NICK wicky");
	ircApp.receive(101, "USER y 0 * :y");

	std::cout << std::endl;
}

void	testJoin(IrcApplicationLayer& ircApp) {
	ircApp.receive(100, "JOIN #testchannel");
}

//**********************

void	testApplicationLayer() {
	IrcApplicationLayer	ircApp = IrcApplicationLayer("password");

	registerTestusers(ircApp);
	testJoin(ircApp);
}