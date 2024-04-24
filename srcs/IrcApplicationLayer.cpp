/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IrcApplicationLayer.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marschul <marschul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/17 17:04:57 by marschul          #+#    #+#             */
/*   Updated: 2024/04/24 19:11:18 by marschul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "IrcApplicationLayer.hpp"
#include "helperFunctions.hpp"
#include "Mode.hpp"
#include <iostream>
#include <arpa/inet.h>
#include <algorithm>

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

	// set host name
	newUser->setHost(inet_ntoa(address.sin_addr));
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
	std::vector<std::string>	channelNames;
	std::vector<std::string>	keys;
	std::string					current;
	Channel 					*channel;
	std::string					key;
	std::vector<int>			members;
	std::string					memberList;

	words = splitString(line, " ");

	// check for correct syntax
	if (words.size() < 2) {
		sendError(user, "461", words[0] + " :Not enough parameters");
	}

	// check if parameter is 0
	if (words[1] == "0") {
		// send PART to all channels the user is member in
		return;
	}

	// get all channels
	channelNames = splitString(words[1], ",");

	// get all keys
	if (words.size() > 2)
		keys = splitString(words[2], ",");
	else
		keys = std::vector<std::string>();

	// iterate over all channels
	for (size_t i = 0; i < channelNames.size(); i++) {
		// set current as current channel name
		current = channelNames[i];

		// check for #
		if ((current)[0] != '#') {
			sendError(user, "403", words[i] + " :No such channel");
		}

		// if channel doesnt exist we create it and add user to operator list
		if (_channels.find(current) == _channels.end()) {
			_channels[current] = new Channel(current);
			_channels[current]->addOperator(user.getId());
		}

		// get channel, add user to channel and add channel to user's channel list
		channel = _channels[current];
		channel->addMember(user.getId());
		user.addChannel(current);

		// check if channel is invite only
		if (channel->getModeI() == true && channel->isInInviteList(user.getId()) == false) {
			sendError(user, "473", current + " :Cannot join channel (+i)");
			return;
		}

		// check key if channel has key
		if (i < keys.size())
			key = keys[i];
		else
			key = "";
		if (channel->getModeK() == true && channel->getKey() != key) {
			sendError(user, "475", current + " :Cannot join channel (+k)");
			return;	
		}

		// check limit if channel has user limit
		if (channel->getModeL() == true && channel->getUserLimit() <= channel->getCurrentMembersize()) {
			sendError(user, "471", current + " :Cannot join channel (+l)");
			return;	
		}
		
		// send join message to all member of the channel
		members = channel->getMembers();
		sendPrefixMessageToMany(user, members, "JOIN", current);

		// send RPL_TOPIC / RPL_NOTOPIC
		if (channel->getModeT() == true)
			sendServerMessage(user, "332", current + " :" + channel->getTopic());
		else
			sendServerMessage(user, "331", current + " :No topic is set");

		// send member list
		memberList = "";
		for (std::vector<int>::iterator it = members.end() - 1; it >= members.begin(); it--) {
			if (channel->isOperator(*it) == true)
				memberList += "@";
			memberList += _users[*it]->getNick();
			memberList += " ";
		}
		sendServerMessage(user, "353", " = " + current + " :" + memberList);
		sendServerMessage(user, "366", current + " :End of NAMES list");
	}
}	

void	IrcApplicationLayer::handlePart(User& user, std::string line) {
	std::vector<std::string>	words;
	std::string					message;
	std::vector<std::string>	channelNames;
	std::string					current;
	Channel 					*channel;
	std::vector<int>			members;

	words = splitString(line, " ");

	// check for correct syntax
	if (words.size() < 2) {
		sendError(user, "461", words[0] + " :Not enough parameters");
	}

	// get message
	message = ":";
	if (words.size() >= 3)
		message = getSeveralWords(words, 2);

	// get all channels
	channelNames = splitString(words[1], ",");

	// iterate over all channels
	for (std::vector<std::string>::iterator it = channelNames.begin(); it < channelNames.end(); it++) {
		// set current to current channel name
		current = *it;

		// check if channel exists
		if (_channels.find(current) == _channels.end()) {
			sendError(user, "403", words[1] + " :No such channel");
			return;
		}

		// check if user is on channel
		channel = _channels[current];
		if (channel->isMember(user.getId()) == false) {
			sendError(user, "442", current + " :You're not on that channel");
			return;
		}
		
		// send response back to user
		members = channel->getMembers();
		sendPrefixMessageToMany(user, members, "PART", current + " :" + message);

		// remove user from channel._members list
		channel->removeMember(user.getId());
	}
}

void	IrcApplicationLayer::handlePrivmsg(User& user, std::string line) {
	std::vector<std::string>	words;
	std::string					message;
	int							recipientId;
	Channel 					*channel;
	std::vector<int>			members;
	std::vector<int>::iterator	it;
	
	words = splitString(line, " ");

	// check for recipient
	if (words.size() < 2) {
		sendError(user, "411", ":No recipient given (PRIVMSG)");
		return;
	}

	// check for message
	if (words.size() < 3) {
		sendError(user, "412", ":No text to send");
		return;
	}
	
	// assemble message
	message = getSeveralWords(words, 2);

	// is it a private message?
	recipientId = getUserIdByName(words[1]);
	if (words[1][0] != '#') {
		if (recipientId == -1) {
			sendError(user, "401", words[1] + " :No such nick/channel");
			return;
		} else {
			sendPrefixMessage(user, *_users[recipientId], "PRIVMSG", words[1] + " " + message);
			return;
		}
	}

	// check if channel exists
	if (_channels.find(words[1]) == _channels.end()) {
		sendError(user, "401", words[1] + " :No such nick/channel");
		return;
	}

	channel = _channels[words[1]];

	// check if member is on channel
	if (channel->isMember(user.getId()) == false) {
		sendError(user, "404", words[1] + " :Cannot send to channel");
		return;
	}

	// send to every member in channel except to user
	members = channel->getMembers();
	it = std::find(members.begin(), members.end(), user.getId());
	members.erase(it);
	sendPrefixMessageToMany(user, members, "PRIVMSG", words[1] + " :" + message);
}

void	IrcApplicationLayer::handleKick(User& user, std::string line) {
	std::vector<std::string>	words;
	std::string					message;
	std::vector<std::string>	channelNames;
	std::vector<std::string>	userNames;
	Channel						*channel;
	std::vector<std::string>	targets;
	int							member;

	words = splitString(line, " ");

	// check for correct syntax
	if (words.size() < 3) {
		sendError(user, "461", words[0] + " :Not enough parameters");
		return;
	}

	// get message if there is one
	if (words.size() >= 4)
		message = getSeveralWords(words, 3);
	else
		message = user.getNick();

	// get channels
	channelNames = splitString(words[1], ",");

	// get users
	userNames = splitString(words[2], ",");

	// is the channel to user rule adhered to?
	if (channelNames.size() != 1 && channelNames.size() != userNames.size()) {
		sendError(user, "461", words[0] + " :Not enough parameters");
		return;
	}

	// iterate over channels
	for (size_t i = 0; i < channelNames.size(); i++) {

		// check if channel exists
		if (_channels.find(channelNames[i]) == _channels.end()) {
			sendError(user, "403", words[1] + " :No such channel");
			return;
		}

		channel = _channels[channelNames[i]];

		// check if user is on channel
		if (channel->isMember(user.getId()) == false) {
			sendError(user, "442", channelNames[i] + " :You're not on that channel");
			return;
		}

		// check for operator rights
		if (channel->isOperator(user.getId()) == false) {
			sendError(user, "482", "<channel> :You're not channel operator");
			return;
		}
		
		// assemble target list
		targets.clear();
		if (channelNames.size() == 1)
			targets = userNames;
		else
			targets.push_back(userNames[i]);

		// iterate over all targets
		for (std::vector<std::string>::iterator it = targets.begin(); it < targets.end(); it++) {
			
			// check if to-be-kicked-out-user exists
			member = getUserIdByName(*it);
			if (member == -1) {
				sendError(user, "401", *it + " :No such nick/channel");
				continue;
			}

			// check if to=be-kicked-out-user is on channel
			if (channel->isMember(member) == false) {
				sendError(user, "441", *it + " " + channelNames[i] + ":They aren't on that channel");
				continue;
			}
			
			// remove recipient from channel
			channel->removeMember(member);
			_users[member]->removeChannel(channelNames[i]);
			
			// send message to all members
			sendPrefixMessageToMany(user, channel->getMembers(), "KICK", channelNames[i] + " " + *it + " :" + message);
		}
	}
}

void	IrcApplicationLayer::handleInvite(User& user, std::string line) {
	std::vector<std::string>	words;
	Channel						*channel;
	int							invitee;

	words = splitString(line, " ");

	// check for correct syntax
	if (words.size() < 3) {
		sendError(user, "461", words[0] + " :Not enough parameters");
		return;
	}

	// check if invitee exists
	if (_users.find(getUserIdByName(words[1])) == _users.end()) {
		sendError(user, "401", words[1] + " :No such nick/channel");
		return;
	}

	// check if channel exists
	if (_channels.find(words[2]) == _channels.end()) {
		sendError(user, "403", words[2] + " :No such channel");
		return;
	}

	// get invitee and channel
	channel = _channels[words[2]];
	invitee = getUserIdByName(words[1]);

	// check if invitee is already in channel
	if (channel->isMember(invitee) == true) {
		sendError(user, "443", words[1] = " " + words[2] + " :is already on channel");
		return;
	}

	// check if user is member in channel
	if (channel->isMember(user.getId()) == false) {
		sendError(user, "442", words[2] + " :You're not on that channel");
		return;
	}

	// check if channel is invite-only and user is operator
	if (channel->getModeI() == true && channel->isOperator(user.getId()) == false) {
		sendError(user, "482", words[2] + " :You're not channel operator");
		return;
	}

	// send sender and invitee a server response
	sendServerMessage(user, "341", words[2] + " " + words[1]);
	sendPrefixMessage(user, *_users[invitee], "INVITE", words[1] + ": " + words[2]);
}

void	IrcApplicationLayer::handleTopic(User& user, std::string line) {
	std::vector<std::string>	words;
	Channel						*channel;
	std::vector<int>			members;

	words = splitString(line, " ");

	// check for correct syntax
	if (words.size() < 2) {
		sendError(user, "461", words[0] + " :Not enough parameters");
		return;
	}
	
	// check if channel exists
	if (_channels.find(words[1]) == _channels.end()) {
		sendError(user, "403", words[1] + " :No such channel");
		return;
	}
	
	channel = _channels[words[1]];

	// check if user is on the channel
	if (channel->isMember(user.getId()) == false) {
		sendError(user, "442", words[1] + " :You're not on that channel");
		return;
	}

	// check if channel has topic mode set and user is operator
	if (channel->getModeT() == true && channel->isOperator(user.getId()) == false) {
		sendError(user, "482", " :You're not channel operator");
		return;
	}

	// if there is no topic parameter given
	if (words.size() < 3) {
		// if topic is set, we return topic
		if (channel->getTopic() != "")
			sendServerMessage(user, "332", words[1] + " :<topic>");
		// else we return RPL_NOTOPIC
		else
			sendServerMessage(user, "331", words[1] + " :No topic is set");
		return;
	}
	
	// if there is topic parameter given, we set topic
	channel->setTopic(getSeveralWords(words, 2));

	// inform all members of topic change
	members = channel->getMembers();
	sendPrefixMessageToMany(user, members, "TOPIC", words[1] + " :" + channel->getTopic());
}

void	IrcApplicationLayer::handleMode(User& user, std::string line) {
	std::vector<std::string>	words;
	Channel						*channel;

	words = splitString(line, " ");

	// check for correct syntax
	if (words.size() < 3) {
		sendError(user, "461", words[0] + " :Not enough parameters");
		return;
	}

	// check if channel exists
	if (_channels.find(words[1]) == _channels.end()) {
		sendError(user, "403", words[2] + " :No such channel");
		return;
	}

	channel = _channels[words[1]];

	// check if user is on the channel
	if (channel->isMember(user.getId()) == false) {
		sendError(user, "442", words[1] + " :You're not on that channel");
		return;
	}

	// check if user is operator
	if (channel->isOperator(user.getId()) == false) {
		sendError(user, "482", words[2] + " :You're not channel operator");
		return;
	}

	// execute single mode changes
	Mode	mode = Mode(*this, user, *channel, line);
	mode.execute();
}

void	IrcApplicationLayer::handleQuit(User& user, std::string line) {
	std::vector<std::string>	words;
	std::string					message;
	std::vector<int>			recipients;
	std::vector<std::string>	channelNames;
	Channel						*channel;
	std::vector<int>			members;

	words = splitString(line, " ");

	// get message if there is one
	if (words.size() >= 2)
		message = getSeveralWords(words, 1);

	// get channel names
	channelNames = user.getChannels();

	// delete user as member from all channels
	for (std::vector<std::string>::iterator it = channelNames.begin(); it < channelNames.end(); it++) {
		channel = _channels[*it];
		channel->removeMember(user.getId());
	}

	// get all recipients
	recipients = std::vector<int>();
	for (std::vector<std::string>::iterator it = channelNames.begin(); it < channelNames.end(); it++) {
		channel = _channels[*it];
		members = channel->getMembers();
		recipients.insert(recipients.end(), members.begin(), members.end());
	}

	// send the quit message to all members of all channels the user was in
	sendPrefixMessageToMany(user, recipients, "QUIT", message);

	// delete user from _users map
	_users.erase(user.getId());
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

void	IrcApplicationLayer::sendPrefixMessage(User& sender, User& receiver, std::string command, std::string text) {
	std::string message;

	message = ":" + sender.getNick() + "!" + sender.getUser() + "@" + sender.getHost() + " " + command + " " + text;
	send(receiver.getId(), message);
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

void	IrcApplicationLayer::sendPrefixMessageToMany(User& user, std::vector<int> ids, std::string command, std::string text) {
	for (std::vector<int>::iterator it = ids.begin(); it < ids.end(); it++)
		sendPrefixMessage(user, *_users[*it], command, text);
}
