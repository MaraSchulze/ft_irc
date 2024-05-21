/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IrcApplicationLayerPublic.cpp                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marschul <marschul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/17 17:04:57 by marschul          #+#    #+#             */
/*   Updated: 2024/05/21 11:35:37 by marschul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "IrcApplicationLayer.hpp"
#include "helperFunctions.hpp"
#include <iostream>
#include <arpa/inet.h>
#include <unistd.h>

IrcApplicationLayer::IrcApplicationLayer(std::string password) : _password(password), _serverName(SERVERNAME) {
	_serverCreationTime = getTime();
	_server = NULL;
}

IrcApplicationLayer::~IrcApplicationLayer() {
	// delete all User and Channel objects that we have newed
	for (std::map<int, User*>::iterator it = _users.begin(); it != _users.end(); it++)
		delete (*it).second;
	for (std::map<std::string, Channel*>::iterator it = _channels.begin(); it != _channels.end(); it++)
		delete (*it).second;
}

void	IrcApplicationLayer::connect(int id, struct sockaddr_in address) {
	User	*newUser;

	// create User object
	newUser = new User(id);

	// check if id is still in use, then disconnect id
	if (_users.find(id) != _users.end()) {
		disconnect(id);
	}

	// add to _users map
	_users[id] = newUser;

	// set host name
	newUser->setHost(inet_ntoa(address.sin_addr));
}

void	IrcApplicationLayer::disconnect(int id) {
	User	*user;

	user = getUser(id);

	if (user == NULL)
		return;

	// disconnects on socket layer
	if (_server != NULL)
		_server->disconnectClient(id);

	// remove user from _users map
	deleteUser(id);
}

void	IrcApplicationLayer::receive(int id, std::string line) {
	User						*user;
	std::vector<std::string>	commandLines;

	// debug
	//std::cout << "[debug received] " << line << std::endl;

	user = getUser(id);
	if (user != NULL) {
		// split into commands
		commandLines = splitString(line, "\r\n");

		// dispatch commands
		for (std::vector<std::string>::iterator it = commandLines.begin(); it < commandLines.end(); it++) {
			dispatchCommand(*user, *it);
		}
	}
	else
		std::cout << "Error: Received a message from a client that doesn't exist. (" << id << ")" << std::endl;
}	

SendQueue&	IrcApplicationLayer::getSendQueue() {
	return _sendQueue;
}

int		IrcApplicationLayer::getUserIdByName(std::string name) {
	User	*current;
	
	for (std::map<int, User*>::iterator it = _users.begin(); it != _users.end(); it++) {
		current = (*it).second;
		if (compareStrings(current->getNick(), name) == true)
			return current->getId();
	}
	return -1;
}

void	IrcApplicationLayer::registerServer(Server *server) {
	_server = server;
}