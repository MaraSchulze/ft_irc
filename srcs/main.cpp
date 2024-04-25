/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marschul <marschul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/17 16:09:12 by marschul          #+#    #+#             */
/*   Updated: 2024/04/25 19:09:06 by marschul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "IrcApplicationLayer.hpp"
#include <iostream>

// debug
#include "testApplicationLayer.hpp"

int main(int argc, char **argv) {
	if (argc != 3) {
		std::cout << "usage: ./ircserv <port> <password>" << std::endl;
		return 1;
	}

	//IrcApplicationLayer ircApp = IrcApplicationLayer(argv[2]);

	// test
	testApplicationLayer();
}
