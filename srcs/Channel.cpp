/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marschul <marschul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/18 16:40:25 by marschul          #+#    #+#             */
/*   Updated: 2024/04/23 21:26:39 by marschul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"
#include <algorithm>

Channel::Channel(std::string name) : _name(name) {}

Channel::~Channel() {}

void	Channel::addMember(int id) {
	_members.push_back(id);
}

void	Channel::removeMember(int id) {
	std::vector<int>::iterator it = _members.begin();

	while (it != _members.end()) {
		if (*it == id) {
			_members.erase(it);
			break;
		}
		else 
			it++;
	}
}

bool	Channel::isMember(int id) {
	if (std::find(_members.begin(), _members.end(), id) != _members.end())
		return true;
	else
		return false;
}

std::vector<int>&	Channel::getMembers() {
	return _members;
}

void	Channel::addOperator(int id) {
	_operators.push_back(id);
}

void	Channel::removeOperator(int id) {
	std::vector<int>::iterator it = _operators.begin();

	while (it != _operators.end()) {
		if (*it == id) {
			_operators.erase(it);
			break;
		}
		else 
			it++;
	}
}

bool	Channel::isOperator(int id) {
	if (std::find(_operators.begin(), _operators.end(), id) != _operators.end())
		return true;
	else
		return false;
}

void	Channel::setModeI(bool i) {
	_modeI = i;
}

bool	Channel::getModeI() {
	return _modeI;
}

void	Channel::setModeK(bool k) {
	_modeK = k;
}

bool	Channel::getModeK() {
	return _modeK;
}

void	Channel::setModeL(bool l) {
	_modeL = l;
}

bool	Channel::getModeL() {
	return _modeL;
}

void	Channel::setModeT(bool t) {
	_modeT = t;
}

bool	Channel::getModeT() {
	return _modeT;
}

std::string	Channel::getKey() {
	return _key;
}

void	Channel::setKey(std::string key) {
	_key = key;
}

void	Channel::setUserLimit(int userLimit) {
	_userLimit = userLimit;
}

int		Channel::getUserLimit() {
	return _userLimit;
}

int		Channel::getCurrentMembersize() {
	return _currentMemberSize;
}

std::string	Channel::getTopic() {
	return _topic;
}

void	Channel::setTopic(std::string topic) {
	_topic = topic;
}

void	Channel::addToInviteList(int id) {
	_inviteList.push_back(id);
}

bool	Channel::isInInviteList(int id) {
	return std::find(_inviteList.begin(), _inviteList.end(), id) != _inviteList.end();
}
