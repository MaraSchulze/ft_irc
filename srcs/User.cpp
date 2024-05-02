/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marschul <marschul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/18 16:38:05 by marschul          #+#    #+#             */
/*   Updated: 2024/05/02 10:41:34 by marschul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "User.hpp"
#include "helperFunctions.hpp"
#include <algorithm>

// debug
#include <iostream>

User::User(int id) : _id(id), _authStatusPass(0), _authStatusNick(0), _authStatusUser(0) {
	std::cout << "[debug] User " << id << " connected" << std::endl;
}

User::~User() {}

int	User::getId() {
	return _id;
}

std::string	User::getHost() {
	return _host;
}

void User::setHost(char *host) {
	_host = std::string(host);
}
void	User::setAuthStatusPass() {
	_authStatusPass = 1;
}

void	User::setAuthStatusNick() {
	_authStatusNick = 1;
}

bool	User::getAuthStatusNick() {
	return _authStatusNick == 1;
}

void	User::setAuthStatusUser() {
	_authStatusUser = 1;
}

bool	User::getAuthStatusUser() {
	return _authStatusUser == 1;
}

/*
If all 3 required commands have been sent, we are registered.
*/
bool	User::checkAuthStatus() {
	return _authStatusPass == 1 && _authStatusNick == 1 && _authStatusUser == 1;
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
	_channels.push_back(setToLower(channel));
}

void	User::removeChannel(std::string channel) {
	std::vector<std::string>::iterator it;

	it = std::find(_channels.begin(), _channels.end(), setToLower(channel));
	if (it != _channels.end())
		_channels.erase(it);
}

std::vector<std::string>&	User::getChannels() {
	return _channels;
}