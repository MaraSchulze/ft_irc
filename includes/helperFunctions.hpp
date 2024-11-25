/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   helperFunctions.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marschul <marschul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/18 22:26:08 by marschul          #+#    #+#             */
/*   Updated: 2024/05/02 10:14:19 by marschul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HELPERFUNCTIONS_HPP
# define HELPERFUNCTIONS_HPP

# include <string>
# include <vector>

std::string	firstWord(std::string line);
std::vector<std::string> splitString(const std::string &str, const std::string& separator);
std::string	getSeveralWords(std::vector<std::string> words, int begin);
bool	isCorrectNick(std::string nick);
std::string	getTime();
int	conversePortIntoInt(char *input);
int	converseIntoInt(std::string str);
bool	compareStrings(std::string a, std::string b);
std::string	setToLower(std::string str);

#endif
