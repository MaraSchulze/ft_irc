/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IrcApplicationLayerHelperFunctions.cpp             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marschul <marschul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/09 18:32:05 by marschul          #+#    #+#             */
/*   Updated: 2024/05/09 21:52:22 by marschul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "IrcApplicationLayer.hpp"
#include "helperFunctions.hpp"

User	*IrcApplicationLayer::getUser(int id) {
	if (_users.find(id) != _users.end())
		return _users[id];
	else
		return NULL;
}

Channel	*IrcApplicationLayer::getChannel(std::string name) {
	if (_channels.find(setToLower(name)) != _channels.end())
		return _channels[setToLower(name)];
	else
		return NULL;
}

void	IrcApplicationLayer::setChannel(std::string name, Channel *channel) {
	_channels[setToLower(name)] = channel;
}

void	IrcApplicationLayer::deleteUser(int id) {
	User						*user;
	Channel						*channel;
	std::vector<std::string>	channels;

	user = getUser(id);
	if (user != NULL) {
		// remove user from all _channels
		channels = user->getChannels();
		for (std::vector<std::string>::iterator it = channels.begin(); it < channels.end(); it++) {
			channel = getChannel(*it);
			if (channel != NULL) {
				channel->removeMember(id);
				if (channel->channelIsEmpty() == true)
					deleteChannel(channel->getName());
			}
		}

		// delete user from _users map
		delete user;
		_users.erase(id);
	}
}

void	IrcApplicationLayer::deleteChannel(std::string name) {
	name = setToLower(name);
	if (_channels.find(name) != _channels.end())
		_channels.erase(name);
}

void	IrcApplicationLayer::deleteMemberFromChannel(Channel *channel, int id) {
	User	*user;

	channel->removeMember(id);
	if (channel->channelIsEmpty())
		deleteChannel(channel->getName());
	user = getUser(id);
	if (user != NULL)
		user->removeChannel(channel->getName());	
}
