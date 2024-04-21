/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marschul <marschul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/18 16:38:05 by marschul          #+#    #+#             */
/*   Updated: 2024/04/21 16:40:53 by marschul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "User.hpp"

// debug
#include <iostream>

User::User(int id) : _id(id) {
	for (int i = 0; i < 3; i ++)
		_authStatus[i] = 0;
	std::cout << "[debug] User " << id << " connected" << std::endl;
}

User::~User() {}

int	User::getId() {
	return _id;
}

std::string	User::getHost() {
	return "www.dummy.com";
}

bool	User::setAuthStatus(int status) {
	// if we already were in that phase of registration, give back that info to the caller
	if (_authStatus[status] == 1)
		return false;

	// set _authStatus
	_authStatus[status] = 1;
	return true;
}

/*
If all 3 required commands have been sent, we are registered.
*/
bool	User::checkAuthStatus() {
	int	sum = 0;

	for (int i = 0; i < 3; i++) {
		if (_authStatus[i] == 1)
			sum++;
	}
	return (sum == 3);
}

void	User::setNick(std::string nick) {
	_nick = nick;
}

std::string	User::getNick() {
	return _nick;
}

void	User::setUser(std::string userName) {
	_user = userName;
}

std::string	User::getUser() {
	return _user;
}

void	User::setRealName(std::string realName) {
	_realName = realName;
}

void	User::addChannel(std::string channel) {
	_channels.push_back(channel);
}

std::vector<std::string>&	User::getChannels() {
	return _channels;
}