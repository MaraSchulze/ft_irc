/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SendQueue.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marschul <marschul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/22 16:42:22 by marschul          #+#    #+#             */
/*   Updated: 2024/04/22 16:59:10 by marschul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "SendQueue.hpp"

// debug
#include <iostream>

SendQueue::SendQueue() {}

SendQueue::~SendQueue() {}

void	SendQueue::push(int id, std::string message) {
	_id.push(id);
	_message.push(message);
	std::cout << "[sendqueue] " << "(id " << id << ", message " << message << ")" << std::endl;
}

void	SendQueue::pop() {
	_id.pop();
	_message.pop();
}

int		SendQueue::getNextSender() {
	return _id.front();
}

std::string	SendQueue::getNextMessage() {
	return _message.front();
}

bool	SendQueue::empty() {
	return _id.empty();
}

size_t	SendQueue::size() {
	return _id.size();
}
