/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IrcApplicationLayer.hpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marschul <marschul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/17 16:39:00 by marschul          #+#    #+#             */
/*   Updated: 2024/04/17 18:30:57 by marschul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRCAPPLICATIONLAYER_HPP
# define IRCAPPLICATIONLAYER_HPP

class  IrcApplicationLayer {
	public:
		IrcApplicationLayer();
		~IrcApplicationLayer();
		void	connect(struct sockaddr_in address);
		void	disconnect(struct sockaddr_in address);
		void	receive(struct sockaddr_in address, std::string line);
		int		getPipeFd();
};

#endif
