/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marschul <marschul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/17 16:09:12 by marschul          #+#    #+#             */
/*   Updated: 2024/05/20 18:06:25 by marschul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "IrcApplicationLayer.hpp"
#include "Server.hpp"
#include <iostream>

int main(int argc, char **argv) {
	if (argc != 3) {
		std::cout << "usage: ./ircserv <port> <password>" << std::endl;
		return 1;
	}

	// Create ircApplicationLayer
	IrcApplicationLayer ircApp = IrcApplicationLayer(argv[2]);

	// Create server
	Server server(6667, ircApp);

	// run server
	server.run();
}
