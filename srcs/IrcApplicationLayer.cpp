/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IrcApplicationLayer.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marschul <marschul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/17 17:04:57 by marschul          #+#    #+#             */
/*   Updated: 2024/04/22 20:02:05 by marschul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "IrcApplicationLayer.hpp"
#include "helperFunctions.hpp"
#include "Mode.hpp"
#include <iostream>

IrcApplicationLayer::IrcApplicationLayer(std::string _password) : _password(_password), _serverName(SERVERNAME) {}

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

SendQueue&	IrcApplicationLayer::getSendQueue() {
	return _sendQueue;
}

int		IrcApplicationLayer::getUserIdByName(std::string name) {
	User	*current;
	
	for (std::map<int, User*>::iterator it = _users.begin(); it != _users.end(); it++) {
		current = (*it).second;
		if (current->getNick() == name)
			return current->getId();
	}
	return -1;
}

//******************** private methods **********************************

void	IrcApplicationLayer::dispatchCommand(User& user, std::string line) {
	std::string	command;
	std::string commands[11] = {"PASS", "NICK", "USER", "JOIN", "PART", "PRIVMSG", "KICK", "INVITE", "TOPIC", "MODE", "QUIT"};
	int	index;

	command = firstWord(line);
	for (index = 0; index < 11; index++) {
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
		case 4	:	handlePart(user, line);
					return;
		case 5	:	handlePrivmsg(user, line);
					return;
		case 6	:	handleKick(user, line);
					return;
		case 7	:	handleInvite(user, line);
					return;
		case 8	:	handleTopic(user, line);
					return;
		case 9	:	handleMode(user, line);
					return;
		case 10	:	handleQuit(user, line);
					return;
		default :	sendError(user, "421", command + " :Unknown command");
	}
}

void	IrcApplicationLayer::handlePass(User& user, std::string line) {
	std::vector<std::string>	words;

	words = splitString(line, " ");

	// check for correct syntax
	if (words.size() != 2) {
		sendError(user, "464", ":Password incorrect");
		return;
	}

	// check if we are already registered
	if (user.checkAuthStatus() == true) {
		sendError(user, "462", ":Unauthorized command (already registered)");
		return;
	}

	// check if this is the first command
	if (user.getAuthStatusNick() == true || user.getAuthStatusUser() == true) {
		sendError(user, "464", ":Password incorrect");
		return;
	}

	// check for right password
	if (_password != words[1]) {
		sendError(user, "464", ":Password incorrect");
		return;
	}

	// set authStatus
	user.setAuthStatusPass();
}	

void	IrcApplicationLayer::handleNick(User& user, std::string line) {
	std::vector<std::string>	words;
	User						*currentUser;
	std::string					nick;
	std::string					currentNick;

	words = splitString(line, " ");

	// check for correct syntax
	if (words.size() < 2) {
		sendError(user, "431", ":No nickname given");
		return;
	}
	
	nick = words[1];

	// check for correct nick
	if (isCorrectNick(nick) == false) {
		sendError(user, "432", nick + " :Erroneous nickname");
		return;
	}

	// check if a NICK command has already been sent in this registration
	if (user.getAuthStatusNick() == true) {
		sendError(user, "462", ":Unauthorized command (already registered)");
		return;
	}
	
	// check if nick is already in use
	for (std::map<int, User*>::iterator it = _users.begin(); it != _users.end(); it++) {
		currentUser = (*it).second;
		currentNick = currentUser->getNick(); 
		if (nick == currentNick) {
			sendError(user, "431", nick + " :Nickname is already in use");
			return;
		}
	}

	user.setNick(nick);
	user.setAuthStatusNick();

	// Is registration complete?
	if (user.checkAuthStatus() == true)
		sendWelcome(user);
}	

void	IrcApplicationLayer::handleUser(User& user, std::string line) {
	std::vector<std::string>	words;
	std::string					realName;

	words = splitString(line, " ");

	// check for correct syntax
	if (words.size() < 5) {
		sendError(user, "461", "USER :Not enough parameters");
		return;
	}
	
	// check if we are already registered
	if (user.getAuthStatusUser() == true) {
		sendError(user, "462", ":Unauthorized command (already registered)");
		return;
	}

	// set user and real name
	user.setUser(words[1]);
	realName = getSeveralWords(words, 4);
	if (realName[0] == ':')
		realName = realName.substr(1);
	user.setRealName(realName);

	// set auth status user
	user.setAuthStatusUser();

	// Is registration complete?
	if (user.checkAuthStatus() == true)
		sendWelcome(user);
}	

void	IrcApplicationLayer::handleJoin(User& user, std::string line) {
	std::vector<std::string>	words;
	Channel *channel;
	std::vector<int>	members;
	std::string					memberList;

	words = splitString(line, " ");

	// check for correct syntax
	if (words.size() < 2) {
		sendError(user, "461", words[0] + " :Not enough parameters");
	}

	for (size_t i = 1; i < words.size(); i++) {
		// check for #
		if (words[i][0] != '#') {
			sendError(user, "403", words[i] + " :No such channel");
		}

		// if channel doesnt exist we create it
		if (_channels.find(words[1]) == _channels.end()) {
			channel = new Channel(words[i]);
			_channels[words[1]] = channel;
			channel->addMember(user.getId());
			// add user as operator
			user.addChannel(words[1]);
		} else {
			// add user to channel
			channel = _channels[words[i]];
			channel->addMember(user.getId());
			user.addChannel(words[1]);
		}
		
		// send response back to user
		members = channel->getMembers();
		memberList = "";
		for (std::vector<int>::iterator it = members.end() - 1; it >= members.begin(); it--) {
			memberList += _users[*it]->getNick();
			memberList += " ";
		}
		sendError(user, "353", " = " + words[i] + " :" + memberList);
		sendError(user, "331", words[1] + " :No topic is set");
	}
}	

void	IrcApplicationLayer::handlePart(User& user, std::string line) {
	std::vector<std::string>	words;
	Channel 					*channel;
	std::vector<int>			members;
	std::string					message;

	words = splitString(line, " ");

	// check for correct syntax
	if (words.size() < 2) {
		sendError(user, "461", words[0] + " :Not enough parameters");
	}

	// get message
	if (words.size() == 3)
		message = words[2];

	for (size_t i = 1; i < words.size(); i++) {
		// check for #
		if (words[i][0] != '#' || _channels.find(words[i]) == _channels.end()) {
			sendError(user, "403", words[1] + " :No such channel");
		}

		// check if user is on channel
		channel = _channels[words[i]];
		if (channel->isMember(user.getId())) {
			sendError(user, "442", words[i] + " :You're not on that channel");
		}
		
		// send response back to user
		members = channel->getMembers();
		for (std::vector<int>::iterator it = members.end() - 1; it >= members.begin(); it--) {
			if (*it != user.getId())
				send(*it, ":" + user.getNick() + "!" + user.getUser() + "@" + user.getHost() + "PART " + words[i] + message);
		}

		// remove user from channel._members list
		channel->removeMember(user.getId());
	}
}

void	IrcApplicationLayer::handlePrivmsg(User& user, std::string line) {
	std::vector<std::string>	words;
	Channel 					*channel;
	std::string					message;
	User						*member;
	int							recipientId;

	words = splitString(line, " ");

	// check for correct syntax

	// check for rights

	// assemble message
	message = words[2];

	// is it a private message?
	recipientId = getUserIdByName(words[1]);
	if (words[1][0] != '#') {
		if (recipientId == -1) {
			sendError(user, "401", words[1] + " :No such nick/channel");
			return;
		} else {
			sendPrefixMessage(user, *_users[recipientId], "PRIVMSG", words[1] + " :" + message);
			return;
		}
	}

	// it seems to be a channel
	if (_channels.find(words[1]) == _channels.end()) {
		sendError(user, "403", words[1] + " :No such channel");
	} else {
		channel = _channels[words[1]];
		for (std::vector<int>::iterator it = channel->getMembers().begin(); it != channel->getMembers().end(); it++) {
			if (*it != user.getId()) {
				member = _users[*it];
				sendPrefixMessage(*member, user, "PRIVMSG", words[1] + " :" + message);
			}
		}
	}
}

void	IrcApplicationLayer::handleKick(User& user, std::string line) {
	std::vector<std::string>	words;
	std::string					message;
	Channel						*channel;
	int							member;

	words = splitString(line, " ");

	// check for correct syntax
	if (words.size() < 3) {
		sendError(user, "461", words[0] + " :Not enough parameters");
		return;
	}

	// get message if there is one
	if (words.size() >= 4)
		message = words[3];

	// check if channel exists
	if (_channels.find(words[1]) == _channels.end()) {
		sendError(user, "403", words[1] + " :No such channel");
		return;
	}
	else
		channel = _channels[words[1]];

	// check for operator rights
	if (channel->isOperator(user.getId()) == false) {
		sendError(user, "482", "<channel> :You're not channel operator");
		return;
	}

	// check if to-be-kicked-out-user exists
	member = getUserIdByName(words[2]);
	if (member == -1) {
		sendError(user, "401", words[2] + " :No such nick/channel");
		return;
	}

	// check if to=be-kicked-out-user is on channel
	if (channel->isMember(member) == false) {
		sendError(user, "441", words[2] + " " + words[1] + ":They aren't on that channel");
		return;
	}
	
	// send message to all members
}

void	IrcApplicationLayer::handleInvite(User& user, std::string line) {
	std::vector<std::string>	words;
	Channel						*channel;

	words = splitString(line, " ");

	// check for correct syntax
	if (words.size() < 3) {
		sendError(user, "461", words[0] + " :Not enough parameters");
		return;
	}

	// check if invitee exists

	// check if channel exists

	// check if user is member in channel

	// check if channel is invite-only and user is operator

	// send invitee a server response
}

void	IrcApplicationLayer::handleTopic(User& user, std::string line) {
	// check for correct syntax

	// check if channel exists

	// check if user is on the channel

	// check if channel has topic mode set and user is operator

	// check if topic is given and send back reply according to that
}

void	IrcApplicationLayer::handleMode(User& user, std::string line) {
	std::vector<std::string>	words;
	Channel						*channel;

	words = splitString(line, " ");
	channel = _channels[words[1]];
	Mode 	mode = Mode(*this, user, *channel, line);

	mode.execute();
}

void	IrcApplicationLayer::handleQuit(User& user, std::string line) {
	// check for correct syntax

	// get message if there is one

	// get nick for return message

	// delete user from _users map

	// delete user as member from all channels

	// send the quit message to all members of all channels the user was in
	// :<username>!user@host QUIT :Quit message here

}

void	IrcApplicationLayer::sendError(User& user, std::string errorcode, std::string errorMessage) {
	std::string message;

	message = ":" + _serverName + " " + errorcode + " " + user.getNick() + " " + errorMessage;
	send(user.getId(), message);
}

void	IrcApplicationLayer::sendServerMessage(User& user, std::string code, std::string text) {
	std::string message;

	message = ":" + _serverName + " " + code + " " + user.getNick() + " " + text;
	send(user.getId(), message);
}

void	IrcApplicationLayer::sendPrefixMessage(User& user, User& sender, std::string command, std::string text) {
	std::string message;

	message = ":" + sender.getNick() + "!" + sender.getUser() + "@" + sender.getHost() + " " + command + " " + text;
	send(user.getId(), message);
}

void	IrcApplicationLayer::sendWelcome(User& user) {
	sendServerMessage(user, "001", "Welcome to the Internet Relay Network " + user.getNick() + "!" + user.getUser() + "@" + user.getHost());
	sendServerMessage(user, "002", "Your host is " + _serverName + ", running version 1.0.0");
	sendServerMessage(user, "003", "This server was created " + getTime());
	sendServerMessage(user, "004", _serverName + " 1.0.0" + " - " + "iklot");
}

void	IrcApplicationLayer::send(int id, std::string message) {
	_sendQueue.push(id, message);
}
