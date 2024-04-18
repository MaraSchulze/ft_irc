/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marschul <marschul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/18 16:31:05 by marschul          #+#    #+#             */
/*   Updated: 2024/04/18 22:23:19 by marschul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef USER_HPP
# define USER_HPP

# include <string>
# include <vector>

class  User {
	private:
		int							_id;
		int							_authStatus;
		std::string					_nick;
		std::string					_user;
		std::vector<std::string>	_channels;
	public:
		User(int id);
		~User();
		int	getId();
		void	setAuthStatus(int status);
		void	setNick(std::string nick);
		std::string	getNick();
		void	setUser(std::string userName);
		void	addChannel(std::string channel);
		std::vector<std::string>&	getChannels();
};

#endif
