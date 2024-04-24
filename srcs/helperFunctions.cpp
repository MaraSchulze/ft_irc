/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   helperFunctions.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marschul <marschul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/18 22:25:05 by marschul          #+#    #+#             */
/*   Updated: 2024/04/24 20:01:33 by marschul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "helperFunctions.hpp"
#include <cctype>
#include <ctime>
#include <algorithm>

std::string	firstWord(std::string line) {
	size_t position;

	position = line.find(" ");
	if (position == std::string::npos)
		position = line.size();
	return line.substr(0, position);
}

std::vector<std::string> splitString(const std::string &str, const std::string& separator) {
    std::vector<std::string> words;
    std::string::size_type start = 0;
    std::string::size_type end = str.find(separator);

    while (end != std::string::npos) {
        if (start != end) {
            words.push_back(str.substr(start, end - start));
        }
        start = end + 1;
        end = str.find(separator, start);
    }
    if (start < str.length()) {
        words.push_back(str.substr(start));
    }
    return words;
}

std::string	getSeveralWords(std::vector<std::string> words, int begin) {
	std::string	result;

	for (size_t i = begin; i < words.size(); i++) {
		result += words[i];
		result += " ";
	}
	if (result.size() > 1 && result[0] == ':')
		result = result.substr(1);
	return result;
}

bool	isCorrectNick(std::string nick) {
	std::string charSet = "01234567890-{}[]\\^`";

	if (isalpha(nick[0]) == false)
		return false;
	for (size_t i = 1; i < nick.size(); i++) {
		if (isalpha(nick[i]) == false && charSet.find(nick[i]) == std::string::npos)
			return false;
	}
	return true;
}

std::string	getTime() {
	time_t 		rawTime;
    struct tm 	*timeInfo;
    char 		buffer[80];
	std::string	result;

	time(&rawTime);
    timeInfo = localtime(&rawTime);
    strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", timeInfo);
	result = buffer;
	return result;
}

int	converseIntoInt(std::string str) {
	unsigned long int	limit;
	
	for (size_t i = 0; i < str.size(); i++) {
		if (isdigit(str[i]) == false)
			return false;
	}
	limit = 0;
	for (int i = 0; i < std::min((int) (str.size()), 10); i++) {
		limit = 10 * limit + (str[i] - '0');
	}
	if (limit < 1 || limit > 2147483647)
		return false;
	return true;
}