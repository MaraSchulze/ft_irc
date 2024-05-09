/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IrcApplicationLayerSendFunctions.cpp               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marschul <marschul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/09 18:31:15 by marschul          #+#    #+#             */
/*   Updated: 2024/05/09 19:45:44 by marschul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "IrcApplicationLayer.hpp"

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

void	IrcApplicationLayer::sendPrefixMessageToMany(User& user, std::vector<int> ids, std::string command, std::string text) {
	User	*receiver;

	for (std::vector<int>::iterator it = ids.begin(); it < ids.end(); it++) {
		receiver = getUser(*it);
		if (receiver != NULL)
			sendPrefixMessage(user, *receiver, command, text);
	}
}

void	IrcApplicationLayer::sendWelcome(User& user) {
	sendServerMessage(user, "001", "Welcome to the Internet Relay Network " + user.getNick() + "!" + user.getUser() + "@" + user.getHost());
	sendServerMessage(user, "002", "Your host is " + _serverName + ", running version 1.0.0");
	sendServerMessage(user, "003", "This server was created " + _serverCreationTime);
	sendServerMessage(user, "004", _serverName + " ircserv-1.0.0" + " oiws " + "obtkmlvsni");
}

void	IrcApplicationLayer::send(int id, std::string message) {
	_sendQueue.push(id, message + '\n');
}
