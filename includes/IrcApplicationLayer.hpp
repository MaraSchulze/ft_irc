/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IrcApplicationLayer.hpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marschul <marschul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/17 16:39:00 by marschul          #+#    #+#             */
/*   Updated: 2024/04/17 20:14:12 by marschul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRCAPPLICATIONLAYER_HPP
# define IRCAPPLICATIONLAYER_HPP

class  IrcApplicationLayer {
	public:
		IrcApplicationLayer();
		~IrcApplicationLayer();
		int		connect(struct sockaddr_in address);	// returns fd of pipe with data to the client,
			// I need the host information (can just be the IP address) and an identifier for the client, see receive method
		void	disconnect(struct sockaddr_in address);
		void	receive(struct sockaddr_in address, std::string line);	// here I need some identification to identify the client,
			// but you can choose the first parameter, if it is the address or the socket fd, how you like it best.
};

#endif
