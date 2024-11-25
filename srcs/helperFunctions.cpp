/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   helperFunctions.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marschul <marschul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/18 22:25:05 by marschul          #+#    #+#             */
/*   Updated: 2024/05/02 10:15:15 by marschul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "helperFunctions.hpp"
#include <cctype>
#include <ctime>
#include <algorithm>

std::string	firstWord(std::string line) {
	size_t 		position;
	std::string	result;

	position = line.find(" ");
	if (position == std::string::npos)
		position = line.size();
	result = line.substr(0, position);
	return result;
}

std::vector<std::string> splitString(const std::string &str, const std::string& separator) {
    std::vector<std::string> words;
    std::string::size_type start = 0;
    std::string::size_type end = str.find(separator);

    while (end != std::string::npos) {
        if (start != end) {
            words.push_back(str.substr(start, end - start));
        }
        start = end + separator.size();
        end = str.find(separator, start);
    }
    if (start < str.length()) {
        words.push_back(str.substr(start));
    }
    return words;
}

std::string	getSeveralWords(std::vector<std::string> words, int begin) {
	std::string	result;

	if (words[begin][0] != ':')
		return words[begin];
	for (size_t i = begin; i < words.size(); i++) {
		result += words[i];
		result += " ";
	}
	result = result.substr(0, result.size() - 1);
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

int	conversePortIntoInt(char *input) {
	unsigned long int	limit;
    std::string         str;
	
    str = std::string(input);
	for (size_t i = 0; i < str.size(); i++) {
		if (isdigit(str[i]) == false)
			return -1;
	}
	limit = 0;
	for (int i = 0; i < std::min((int) (str.size()), 10); i++) {
		limit = 10 * limit + (str[i] - '0');
	}
	if (limit < 1 || limit > 2147483647)
		return -1;
	return limit;
}

int	converseIntoInt(std::string str) {
	unsigned long int	limit;
	
	for (size_t i = 0; i < str.size(); i++) {
		if (isdigit(str[i]) == false)
			return -1;
	}
	limit = 0;
	for (int i = 0; i < std::min((int) (str.size()), 10); i++) {
		limit = 10 * limit + (str[i] - '0');
	}
	if (limit < 1 || limit > 2147483647)
		return -1;
	return limit;
}

bool	compareStrings(std::string a, std::string b) {
	for (size_t i = 0; i < a.size(); i++)
		a[i] = tolower(a[i]);
	for (size_t i = 0; i < b.size(); i++)
		b[i] = tolower(b[i]);
	return a == b;
}

std::string	setToLower(std::string str) {
	for (size_t i = 0; i < str.size(); i++)
		str[i] = tolower(str[i]);
	return str;
}
