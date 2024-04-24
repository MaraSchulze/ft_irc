/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Mode.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marschul <marschul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/22 15:05:13 by marschul          #+#    #+#             */
/*   Updated: 2024/04/24 21:14:03 by marschul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Mode.hpp"
#include "helperFunctions.hpp"
#include <stdlib.h>

// debug
#include <iostream>

Mode::Mode(IrcApplicationLayer& ircApp, User& user, Channel& channel, const std::string line) : _ircApp(ircApp), _user(user), _channel(channel), _line(line) {

}

Mode::~Mode() {}

void	Mode::execute() {
	char				sign;
	char				flag;
	std::string			flagStr;
	std::vector<int>	members;
	
	// parse
	if (parse() == false) {
		_ircApp.sendError(_user, "461", "MODE :Not enough parameters");
		return;
	}

	// iterate over modes
	for (size_t i = 0; i < _modes.size(); i++) {
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

	// send server messages
	flagStr = "";
	for (std::vector<std::string>::iterator it = _modes.begin(); it < _modes.end(); it++) {
		flagStr += *it;
	}
	members = _channel.getMembers();
	_ircApp.sendPrefixMessageToMany(_user, members, "MODE", _channel.getName() + " " + flagStr);
}

bool	Mode::parse() {
	std::string 				sign;
	std::vector<std::string>	words;
	size_t						flagWordIndex;
	std::string					current;
	size_t						parameterWordIndex;
	size_t						i;
	std::string					mode;

	words = splitString(_line, " ");
	flagWordIndex = 2;
	while (flagWordIndex < words.size()) {
		current = words[flagWordIndex];
		sign = std::string(1, current[0]);
		if (sign != "+" && sign != "-")
			return false;
		parameterWordIndex = flagWordIndex + 1;
		i = 1;
		while (i < current.size()) {
			mode = sign + current[i];
			_modes.push_back(mode);
			if (mode == "+k" || mode == "+l" || mode == "+o" || mode == "-o") {
				if (parameterWordIndex == words.size() || words[parameterWordIndex][0] == '-' || words[parameterWordIndex][0] == '+')
					return false;
				else
					_parameters.push_back(words[parameterWordIndex++]);
			}
			i++;
		}
		flagWordIndex = parameterWordIndex;
	}
	return true;
}

void	Mode::invite(char sign, std::string parameter) {
	// modify parameter for compiler
	(void) parameter;

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

	limit = converseIntoInt(parameter);

	// check if limit is a positive integer
	if (limit == -1) {
		_ircApp.sendError(_user, "461", "MODE :Not enough parameters");
		return;
	}

	if (sign == '+') {
		_channel.setModeL(true);
		_channel.setUserLimit(limit);
	}
	else
		_channel.setModeL(false);
}

void	Mode::op(char sign, std::string parameter) {
	int		userId;

	userId = _ircApp.getUserIdByName(parameter);

	// check if target user is in channel
	if (_channel.isMember(userId) == false) {
		_ircApp.sendError(_user, "441", _user.getNick() + " " + _channel.getName() + " :They aren't on that channel");
		return;
	}

	if (sign == '+')
		_channel.addOperator(userId);
	else
		_channel.removeOperator(userId);
}

void	Mode::topic(char sign, std::string parameter) {
	// modify parameter for compiler
	(void) parameter;
	
	if (sign == '+')
		_channel.setModeT(true);
	else
		_channel.setModeT(false);
}
