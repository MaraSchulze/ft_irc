/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IrcApplicationLayer.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marschul <marschul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/17 17:04:57 by marschul          #+#    #+#             */
/*   Updated: 2024/04/18 23:02:07 by marschul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "IrcApplicationLayer.hpp"
#include "helperFunctions.hpp"
#include <iostream>

IrcApplicationLayer::IrcApplicationLayer(std::string _password) : _password(_password) {}

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
}

void	IrcApplicationLayer::disconnect(int id) {
	User						*user;
	std::vector<std::string>	channels;
	Channel						*channel;

	if (_users.find(id) != _users.end()) {
		user = _users[id];
		// remove user from all _channels
		channels = user->getChannels();
		for (std::vector<std::string>::iterator it = channels.begin(); it < channels.end(); it++) {
			channel = _channels[*it];
			channel->removeMember(id);
		}
		// remove user from _users map
		_users.erase(id);
		std::cout << "[debug] User " << id << " disconnected" << std::endl;
	}
}

void	IrcApplicationLayer::receive(int id, std::string line) {
	User	*user;
	
	// Lookup user with id
	if (_users.find(id) != _users.end()) {
		user = _users[id];
		// dispatch command
		dispatchCommand(*user, line);
	} 
	else
		std::cout << "Error: Received a message from a client that doesn't exist." << std::endl;
}	

//******************** private methods **********************************

void	IrcApplicationLayer::dispatchCommand(User& user, std::string line) {
	std::string	command;
	std::string commands[5] = {"PASS", "NICK", "USER", "JOIN", "PRIVMSG"};
	int	index;

	command = firstWord(line);
	for (index = 0; index < 5; index++) {
		if (command == commands[index])
			break;
	}
	switch (index) {
		case 0 	: 	handlePass(user, line);
					return;
		case 1 	:	handleNick(user, line);
					return;
		case 2	:	handleUser(user, line);
					return;
		case 3	:	handleJoin(user, line);
					return;
		case 4	:	handlePrivmsg(user, line);
					return;
		default :	sendError(user, "421", command + " :Unknown command");
	}
}

void	IrcApplicationLayer::handlePass(User& user, std::string line) {
	std::vector<std::string>	words;

	words = splitString(line);

	// check for correct syntax

	if (_password == words[1])
		user.setAuthStatus(1);
	else {
		std::cout << "Wrong password" << std::endl;
		// send error code back
	}

}	

void	IrcApplicationLayer::handleNick(User& user, std::string line) {
	std::vector<std::string>	words;

	words = splitString(line);

	// check for correct syntax

	user.setNick(words[1]);
	user.setAuthStatus(2);
}	

void	IrcApplicationLayer::handleUser(User& user, std::string line) {
	std::vector<std::string>	words;

	words = splitString(line);

	// check for correct syntax

	user.setUser(words[1]);
	user.setAuthStatus(3);
}	

void	IrcApplicationLayer::handleJoin(User& user, std::string line) {
	std::vector<std::string>	words;
	Channel *channel;

	words = splitString(line);

	// check for correct syntax

	// if channel doesnt exist we create it
	if (_channels.find(words[1]) == _channels.end()) {
		channel = new Channel(words[1]);
		_channels[words[1]] = channel;
		// add user as operator
	} else {
		// add user to channel
		channel = _channels[words[1]];
		channel->addMember(user.getId());
		user.addChannel(words[1]);
	}
	// send response back to user
	std::cout << "[debug] " << user.getNick() << " has joined channel " << words[1] << std::endl;
}	

void	IrcApplicationLayer::handlePrivmsg(User& user, std::string line) {
	std::vector<std::string>	words;
	Channel *channel;
	std::string	message;
	User	*member;

	words = splitString(line);

	// check for correct syntax

	// check for rights
	channel = _channels[words[1]];

	// assemble message
	message = words[2];

	// assemble all recipients and send
	for (std::vector<int>::iterator it = channel->getMembers().begin(); it != channel->getMembers().end(); it++) {
		if (*it != user.getId()) {
			member = _users[*it];
			send(*it, message);
		}
	}
}

void	IrcApplicationLayer::sendError(User& user, std::string errorcode, std::string errorMessage) {
	std::string message;

	message = ":server " + errorcode + " " + user.getNick() + " " + errorMessage;
	send(user.getId(), message);
}

void	IrcApplicationLayer::send(int id, std::string message) {
	std::cout << "Send to " << id << " -> " << message << std::endl;
}