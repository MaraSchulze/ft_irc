/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   helperFunctions.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marschul <marschul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/18 22:26:08 by marschul          #+#    #+#             */
/*   Updated: 2024/04/20 13:55:01 by marschul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HELPERFUNCTIONS_HPP
# define HELPERFUNCTIONS_HPP

# include <string>
# include <vector>

std::string	firstWord(std::string line);
std::vector<std::string> splitString(const std::string &str);
std::string	getSeveralWords(std::vector<std::string> words, int begin);
bool	isCorrectNick(std::string nick);

#endif
