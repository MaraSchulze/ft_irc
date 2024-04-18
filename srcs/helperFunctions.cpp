/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   helperFunctions.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marschul <marschul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/18 22:25:05 by marschul          #+#    #+#             */
/*   Updated: 2024/04/18 22:31:24 by marschul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "helperFunctions.hpp"

std::string	firstWord(std::string line) {
	size_t position;

	position = line.find(" ");
	if (position == std::string::npos)
		position = line.size();
	return line.substr(0, position);
}

// This could be a problem, since we return a local var, not a pointer
std::vector<std::string> splitString(const std::string &str) {
    std::vector<std::string> words;
    std::string::size_type start = 0;
    std::string::size_type end = str.find(" ");

    while (end != std::string::npos) {
        if (end != start) {
            words.push_back(str.substr(start, end - start));
        }
        start = end + 1;
        end = str.find(" ", start);
    }
    if (start < str.length()) {
        words.push_back(str.substr(start));
    }
    return words;
}
