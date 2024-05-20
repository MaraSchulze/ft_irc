/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IrcApplicationLayerCommands.cpp                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marschul <marschul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/09 18:29:02 by marschul          #+#    #+#             */
/*   Updated: 2024/05/20 22:15:28 by marschul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "IrcApplicationLayer.hpp"
#include "helperFunctions.hpp"
#include "Mode.hpp"
#include <iostream>
#include <arpa/inet.h>
#include <algorithm>

void	IrcApplicationLayer::dispatchCommand(User& user, std::string line) {
	std::string	command;
	std::string commands[14] = {"PASS", "NICK", "USER", "JOIN", "PART", "PRIVMSG", "KICK", "INVITE", "TOPIC", "MODE", "QUIT", "CAP", "PING", "NOTICE"};
	int	index;

	command = firstWord(line);
	for (index = 0; index < 14; index++) {
		if (compareStrings(command, commands[index]) == true)
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
		case 11 :	handleCap(user, line);
					return;
		case 12	:	handlePing(user, line);
					return;
		case 13	:	handleNotice(user, line);
					return;		
		default :	sendError(user, "421", command + " :Unknown command");
	}
}

void	IrcApplicationLayer::handlePass(User& user, std::string line) {
	std::vector<std::string>	words;

	words = splitString(line, " ");

	// check for correct syntax
	if (words.size() < 2) {
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
	if (_password != getSeveralWords(words, 1)) {
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
	// check if nick is already in use
	for (std::map<int, User*>::iterator it = _users.begin(); it != _users.end(); it++) {
		currentUser = (*it).second;
		currentNick = currentUser->getNick(); 
		if (compareStrings(nick, currentNick) == true) {
			sendError(user, "431", nick + " :Nickname is already in use");
			return;
		}
	}

	// if we are already registered, send acknowledgement, change nick and return
	if (user.checkAuthStatus() == true) {
		sendPrefixMessage(user, user, "NICK", words[1]);
		user.setNick(words[1]);
		return;
	}
	
	// set new nick
	user.setNick(nick);
	
	// set auth status nick and if registration is complete, send welcome messages
	user.setAuthStatusNick();
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

	// check if user is registered
	if (user.checkAuthStatus() == false) {
		sendError(user, "451", ":You have not registered");
		return;
	}

	// check for correct syntax
	if (words.size() < 2) {
		sendError(user, "461", words[0] + " :Not enough parameters");
	}

	// check if parameter is 0
	if (words[1] == "0") {
		// send PART to all channels the user is member in
		channelNames = user.getChannels();
		for (std::vector<std::string>::iterator it = channelNames.begin(); it < channelNames.end(); it++) {
			handlePart(user, "PART " + *it);
		}
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
			sendError(user, "403", current + " :No such channel");
			continue;
		}

		channel = getChannel(current);

		// if channel doesnt exist we create it and add user to operator list
		if (channel == NULL) {
			setChannel(current, new Channel(current));
			channel = getChannel(current);
			channel->addOperator(user.getId());

		}

		// check if user is already on channel
		if (channel->isMember(user.getId()))
			return;

		// check if channel is invite only
		if (channel->getModeI() == true && channel->isInInviteList(user.getId()) == false) {
			sendError(user, "473", channel->getName() + " :Cannot join channel (+i)");
			continue;
		}

		// check key if channel has key
		if (i < keys.size())
			key = keys[i];
		else
			key = "";
		if (channel->getModeK() == true && channel->getKey() != key) {
			sendError(user, "475", channel->getName() + " :Cannot join channel (+k)");
			continue;	
		}

		// check limit if channel has user limit
		if (channel->getModeL() == true && channel->memberSizeIsReached()) {
			sendError(user, "471", channel->getName() + " :Cannot join channel (+l)");
			continue;	
		}

		// add user to channel and add channel to user's channel list
		channel->addMember(user.getId());
		channel->removeFromInviteList(user.getId());
		user.addChannel(channel->getName());

		// send join message to all member of the channel
		members = channel->getMembers();
		sendPrefixMessageToMany(user, members, "JOIN", channel->getName());

		// send RPL_TOPIC
		if (channel->getTopic() != "")
			sendServerMessage(user, "332", channel->getName() + " :" + channel->getTopic());

		// send member list
		members = channel->getMembers();
		memberList = "";
		for (std::vector<int>::iterator it = members.end() - 1; it >= members.begin(); it--) {
			if (channel->isOperator(*it) == true)
				memberList += "@";
			memberList += _users[*it]->getNick();
			memberList += " ";
		}
		memberList = memberList.substr(0, memberList.size() - 1);
		sendServerMessage(user, "353", "= " + channel->getName() + " :" + memberList);
		sendServerMessage(user, "366", channel->getName() + " :End of NAMES list");
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

	// check if user is registered
	if (user.checkAuthStatus() == false) {
		sendError(user, "451", ":You have not registered");
		return;
	}

	// check for correct syntax
	if (words.size() < 2) {
		sendError(user, "461", words[0] + " :Not enough parameters");
	}

	// get message
	message = "";
	if (words.size() >= 3)
		message = getSeveralWords(words, 2);

	// get all channels
	channelNames = splitString(words[1], ",");

	// iterate over all channels
	for (std::vector<std::string>::iterator it = channelNames.begin(); it < channelNames.end(); it++) {
		// set current to current channel name
		current = *it;

		channel = getChannel(current);

		// check if channel exists
		if (channel == NULL) {
			sendError(user, "403", current + " :No such channel");
			return;
		}

		// check if user is on channel
		if (channel->isMember(user.getId()) == false) {
			sendError(user, "442", channel->getName() + " :You're not on that channel");
			return;
		}
		
		// send response back to user
		members = channel->getMembers();
		sendPrefixMessageToMany(user, members, "PART", channel->getName() + " :" + message);

		// remove user from channel._members list
		deleteMemberFromChannel(channel, user.getId());
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

	// check if user is registered
	if (user.checkAuthStatus() == false) {
		sendError(user, "451", ":You have not registered");
		return;
	}

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

	channel = getChannel(words[1]);

	// check if channel exists
	if (channel == NULL) {
		sendError(user, "401", words[1] + " :No such nick/channel");
		return;
	}

	// check if member is on channel
	if (channel->isMember(user.getId()) == false) {
		sendError(user, "404", channel->getName() + " :Cannot send to channel");
		return;
	}

	// send to every member in channel except to user
	members = channel->getMembers();
	it = std::find(members.begin(), members.end(), user.getId());
	members.erase(it);
	sendPrefixMessageToMany(user, members, "PRIVMSG", channel->getName() + " :" + message);
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

	// check if user is registered
	if (user.checkAuthStatus() == false) {
		sendError(user, "451", ":You have not registered");
		return;
	}

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

		channel = getChannel(channelNames[i]);

		// check if channel exists
		if (channel == NULL) {
			sendError(user, "403", channelNames[i] + " :No such channel");
			return;
		}

		// check if user is on channel
		if (channel->isMember(user.getId()) == false) {
			sendError(user, "442", channel->getName() + " :You're not on that channel");
			return;
		}

		// check for operator rights
		if (channel->isOperator(user.getId()) == false) {
			sendError(user, "482", channel->getName() + " :You're not channel operator");
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
				sendError(user, "441", *it + " " + channel->getName() + ":They aren't on that channel");
				continue;
			}
				
			// send message to all members
			sendPrefixMessageToMany(user, channel->getMembers(), "KICK", channel->getName() + " " + *it + " :" + message);
			
			// remove recipient from channel
			deleteMemberFromChannel(channel, member);
		}
	}
}

void	IrcApplicationLayer::handleInvite(User& user, std::string line) {
	std::vector<std::string>	words;
	Channel						*channel;
	int							invitee;

	words = splitString(line, " ");

	// check if user is registered
	if (user.checkAuthStatus() == false) {
		sendError(user, "451", ":You have not registered");
		return;
	}

	// check for correct syntax
	if (words.size() < 3) {
		sendError(user, "461", words[0] + " :Not enough parameters");
		return;
	}

	// get invitee and channel
	channel = getChannel(words[2]);
	invitee = getUserIdByName(words[1]);
	
	// check if invitee exists
	if (invitee == -1) {
		sendError(user, "401", words[1] + " :No such nick/channel");
		return;
	}

	// check if channel exists
	if (channel == NULL) {
		sendError(user, "403", channel->getName() + " :No such channel");
		return;
	}

	// check if invitee is already in channel
	if (channel->isMember(invitee) == true) {
		sendError(user, "443", words[1] + " " + channel->getName() + " :is already on channel");
		return;
	}

	// check if user is member in channel
	if (channel->isMember(user.getId()) == false) {
		sendError(user, "442", channel->getName() + " :You're not on that channel");
		return;
	}

	// check if channel is invite-only and user is operator
	if (channel->getModeI() == true && channel->isOperator(user.getId()) == false) {
		sendError(user, "482", channel->getName() + " :You're not channel operator");
		return;
	}

	// send sender and invitee a server response
	channel->addToInviteList(invitee);
	sendServerMessage(user, "341", words[1] + " " + channel->getName());
	sendPrefixMessage(user, *_users[invitee], "INVITE", words[1] + " " + channel->getName());
}

void	IrcApplicationLayer::handleTopic(User& user, std::string line) {
	std::vector<std::string>	words;
	Channel						*channel;
	std::vector<int>			members;

	words = splitString(line, " ");

	// check if user is registered
	if (user.checkAuthStatus() == false) {
		sendError(user, "451", ":You have not registered");
		return;
	}

	// check for correct syntax
	if (words.size() < 2) {
		sendError(user, "461", words[0] + " :Not enough parameters");
		return;
	}
	
	channel = getChannel(words[1]);

	// check if channel exists
	if (channel == NULL) {
		sendError(user, "403", words[1] + " :No such channel");
		return;
	}
	
	// check if user is on the channel
	if (channel->isMember(user.getId()) == false) {
		sendError(user, "442", channel->getName() + " :You're not on that channel");
		return;
	}

	// if there is no topic parameter given
	if (words.size() < 3) {
		// if topic is set, we return topic
		if (channel->getTopic() != "")
			sendServerMessage(user, "332", channel->getName() + " :" + channel->getTopic());
		// else we return RPL_NOTOPIC
		else
			sendServerMessage(user, "331", channel->getName() + " :No topic is set");
		return;
	}

	// check if channel has topic mode set and user is operator
	if (channel->getModeT() == true && channel->isOperator(user.getId()) == false) {
		sendError(user, "482", channel->getName() + " :You're not channel operator");
		return;
	}

	// if there is topic parameter given, we set topic
	channel->setTopic(getSeveralWords(words, 2));

	// inform all members of topic change
	members = channel->getMembers();
	sendPrefixMessageToMany(user, members, "TOPIC", channel->getName() + " :" + channel->getTopic());
}

void	IrcApplicationLayer::handleMode(User& user, std::string line) {
	std::vector<std::string>	words;
	Channel						*channel;

	// debug
	//std::cout << "[debug mode] " + line << std::endl;

	words = splitString(line, " ");

	// check if user is registered
	if (user.checkAuthStatus() == false) {
		sendError(user, "451", ":You have not registered");
		return;
	}

	// check for correct syntax
	if (words.size() < 2) {
		sendError(user, "461", words[0] + " :Not enough parameters");
		return;
	}

	channel = getChannel(words[1]);

	// check if channel exists
	if (channel == NULL) {
		sendError(user, "403", words[1] + " :No such channel");
		return;
	}

	// check if no mode flags are set, that is if the client queries the mode flags
	if (words.size() < 3) {
		sendServerMessage(user, "324", channel->getName() + " " + channel->getModeString());
		return;
	}

	// check if user is on the channel
	if (channel->isMember(user.getId()) == false) {
		sendError(user, "442", channel->getName() + " :You're not on that channel");
		return;
	}

	// check if user is operator
	if (channel->isOperator(user.getId()) == false) {
		sendError(user, "482", channel->getName() + " :You're not channel operator");
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

	// check if user is registered
	if (user.checkAuthStatus() == false) {
		sendError(user, "451", ":You have not registered");
		return;
	}
	
	// get message if there is one
	if (words.size() >= 2)
		message = getSeveralWords(words, 1);
	else
		message = "";

	// get channel names
	channelNames = user.getChannels();

	// delete user as member from all channels
	for (std::vector<std::string>::iterator it = channelNames.begin(); it < channelNames.end(); it++) {
		channel = getChannel(*it);
		if (channel != NULL) {
			deleteMemberFromChannel(channel, user.getId());
		}
	}

	// get all recipients
	recipients = std::vector<int>();
	for (std::vector<std::string>::iterator it = channelNames.begin(); it < channelNames.end(); it++) {
		channel = getChannel(*it);
		if (channel != NULL) {
			members = channel->getMembers();
			recipients.insert(recipients.end(), members.begin(), members.end());
		}		
	}

	// send the quit message to all members of all channels the user was in
	sendPrefixMessageToMany(user, recipients, "QUIT", message);

	// disconnect
	disconnect(user.getId());
}

void	IrcApplicationLayer::handleCap(User& user, std::string line) {
	std::vector<std::string>	words;

	words = splitString(line, " ");

	if (words[1] == "LS")
		send(user.getId(), std::string("CAP * LS :"));
	//send(user.getId(), "CAP * END");
}

void	IrcApplicationLayer::handlePing(User& user, std::string line) {
	std::vector<std::string>	words;

	words = splitString(line, " ");

	send(user.getId(), std::string("PONG ") + words[1]);
}

void	IrcApplicationLayer::handleNotice(User& user, std::string line) {
	std::vector<std::string>	words;
	std::string					message;
	int							recipientId;
	Channel 					*channel;
	std::vector<int>			members;
	std::vector<int>::iterator	it;
	
	words = splitString(line, " ");

	// check if user is registered, but don't send back error message
	if (user.checkAuthStatus() == false) {
		return;
	}

	// check for recipient, but don't send back error message
	if (words.size() < 2) {
		return;
	}

	// check for message, but don't send back error message
	if (words.size() < 3) {
		return;
	}
	
	// assemble message
	message = getSeveralWords(words, 2);

	// is it a private message?
	recipientId = getUserIdByName(words[1]);
	if (words[1][0] != '#') {
		if (recipientId == -1) {
			return;
		} else {
			sendPrefixMessage(user, *_users[recipientId], "NOTICE", words[1] + " " + message);
			return;
		}
	}

	channel = getChannel(words[1]);

	// check if channel exists, but don't send back error message
	if (channel == NULL) {
		return;
	}

	// check if member is on channel, but don't send back error message
	if (channel->isMember(user.getId()) == false) {
		return;
	}

	// send to every member in channel except to user
	members = channel->getMembers();
	it = std::find(members.begin(), members.end(), user.getId());
	members.erase(it);
	sendPrefixMessageToMany(user, members, "NOTICE", channel->getName() + " :" + message);
}
