/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marschul <marschul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/18 16:40:25 by marschul          #+#    #+#             */
/*   Updated: 2024/04/18 22:35:06 by marschul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

Channel::Channel(std::string name) : _name(name) {}

Channel::~Channel() {}

void	Channel::addMember(int id) {
	_members.push_back(id);
}

std::vector<int>&	Channel::getMembers() {
	return _members;
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