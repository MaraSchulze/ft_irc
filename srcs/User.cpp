/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marschul <marschul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/18 16:38:05 by marschul          #+#    #+#             */
/*   Updated: 2024/04/18 22:34:14 by marschul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "User.hpp"

// debug
#include <iostream>

User::User(int id) : _id(id), _authStatus(0) {
	std::cout << "[debug] User " << id << " connected" << std::endl;
}

User::~User() {}

int	User::getId() {
	return _id;
}

void	User::setAuthStatus(int status) {
	_authStatus = status;
	std::cout << "[debug] Auth status is now " << _authStatus << std::endl;
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

void	User::addChannel(std::string channel) {
	_channels.push_back(channel);
}

std::vector<std::string>&	User::getChannels() {
	return _channels;
}