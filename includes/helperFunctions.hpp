/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   helperFunctions.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marschul <marschul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/18 22:26:08 by marschul          #+#    #+#             */
/*   Updated: 2024/04/22 13:41:02 by marschul         ###   ########.fr       */
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

#endif
