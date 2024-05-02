/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Mode.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marschul <marschul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/22 14:57:30 by marschul          #+#    #+#             */
/*   Updated: 2024/05/01 21:41:58 by marschul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MODE_HPP
# define MODE_HPP

# include "IrcApplicationLayer.hpp"
# include "User.hpp"
# include "Channel.hpp"
# include <string>
# include <vector>

class  Mode {
	private:
		IrcApplicationLayer&		_ircApp;
		User&						_user;
		Channel&					_channel;
		std::string					_line;
		std::vector<std::string>	_modes;
		std::vector<std::string>	_parameters;

		bool	parse();
		void	invite(char sign, std::string parameter);
		void	key(char sign, std::string parameter);
		bool	limit(char sign, std::string parameter);
		bool	op(char sign, std::string parameter);
		void	topic(char sign, std::string parameter);
	public:
		Mode(IrcApplicationLayer& ircApp, User& user, Channel& channel, const std::string line);
		~Mode();
		void	execute();
};

#endif
