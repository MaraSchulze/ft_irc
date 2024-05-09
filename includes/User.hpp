/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marschul <marschul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/18 16:31:05 by marschul          #+#    #+#             */
/*   Updated: 2024/05/09 19:53:18 by marschul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef USER_HPP
# define USER_HPP

# include <string>
# include <vector>

class  User {
	private:
		int							_id;
		int							_authStatusPass;
		int							_authStatusNick;
		int							_authStatusUser;
		std::string					_nick;
		std::string					_user;
		std::string					_host;
		std::vector<std::string>	_channels;
	public:
		User(int id);
		~User();
		int							getId();
		void						setHost(char *host);
		std::string					getHost();
		void						setAuthStatusPass();
		void						setAuthStatusNick();
		bool						getAuthStatusNick();
		void						setAuthStatusUser();
		bool						getAuthStatusUser();
		bool						checkAuthStatus();
		void						setNick(std::string nick);
		std::string					getNick();
		void						setUser(std::string userName);
		std::string					getUser();
		void						addChannel(std::string channel);
		void						removeChannel(std::string channel);
		std::vector<std::string>&	getChannels();
};

#endif
