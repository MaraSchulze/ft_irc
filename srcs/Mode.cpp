/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Mode.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marschul <marschul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/22 15:05:13 by marschul          #+#    #+#             */
/*   Updated: 2024/04/22 16:27:07 by marschul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Mode.hpp"
#include "helperFunctions.hpp"
#include <stdlib.h>

// debug
#include <iostream>

Mode::Mode(IrcApplicationLayer& ircApp, User& user, Channel& channel, const std::string line) : _ircApp(ircApp), _user(user), _channel(channel), _line(line), _size(0) {

}

Mode::~Mode() {}

void	Mode::execute() {
	char	sign;
	char	flag;
	
	for (int i = 0; i < _size; i++) {
		sign = _modes[i][0];
		flag = _modes[i][1];
		switch (flag) {
			case 'i' :	invite(sign, _parameters[i]);
						break;
			case 'k' :	key(sign, _parameters[i]);
						break;
			case 'l' :	limit(sign, _parameters[i]);
						break;	
			case 'o' :	op(sign, _parameters[i]);
						break;
			case 't' :	topic(sign, _parameters[i]);
						break;
		}
	}
}

void	Mode::parse() {

}

void	Mode::invite(char sign, std::string parameter) {
	if (sign == '+')
		_channel.setModeI(true);
	else
		_channel.setModeI(false);
}

void	Mode::key(char sign, std::string parameter) {
	if (sign == '+') {
		_channel.setModeK(true);
		_channel.setKey(parameter);
	}
	else
		_channel.setModeK(false);
}

void	Mode::limit(char sign, std::string parameter) {
	int	limit;

	if (sign == '+') {
		_channel.setModeL(true);
		limit = atoi(parameter.c_str());
		_channel.setUserLimit(limit);
	}
	else
		_channel.setModeL(false);
}

void	Mode::op(char sign, std::string parameter) {
	int	userId;

	userId = _ircApp.getUserIdByName(parameter);
	if (sign == '+')
		_channel.addOperator(userId);
	else
		_channel.removeOperator(userId);
}

void	Mode::topic(char sign, std::string parameter) {
	if (sign == '+')
		_channel.setModeT(true);
	else
		_channel.setModeT(false);
}
