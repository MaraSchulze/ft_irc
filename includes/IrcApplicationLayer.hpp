/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IrcApplicationLayer.hpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marschul <marschul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/17 16:39:00 by marschul          #+#    #+#             */
/*   Updated: 2024/04/22 19:34:45 by marschul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRCAPPLICATIONLAYER_HPP
# define IRCAPPLICATIONLAYER_HPP

# include "User.hpp"
# include "Channel.hpp"
# include "SendQueue.hpp"
# include <map>
# include <string>
# include <netinet/in.h>
# include <vector>

# define SERVERNAME "irc.rockingpiggies.com"

class  IrcApplicationLayer {
	private:
		std::map<int, User*>			_users;
		std::map<std::string, Channel*>	_channels;
		std::string						_password;
		std::string						_serverName;
		SendQueue						_sendQueue;

		void	dispatchCommand(User& user, std::string line);
		void	handlePass(User& user, std::string line);
		void	handleNick(User& user, std::string line);
		void	handleUser(User& user, std::string line);
		void	handleJoin(User& user, std::string line);
		void	handlePart(User& user, std::string line);
		void	handlePrivmsg(User& user, std::string line);
		void	handleKick(User& user, std::string line);
		void	handleInvite(User& user, std::string line);
		void	handleTopic(User& user, std::string line);
		void	handleMode(User& user, std::string line);
		void	handleQuit(User& user, std::string line);
		void	sendError(User& user, std::string errorcode, std::string errorMessage);
		void	sendServerMessage(User& user, std::string code, std::string text);
		void	sendPrefixMessage(User& user, User& sender, std::string command, std::string text);
		void	send(int id, std::string message);
		void	sendWelcome(User& user);
	public:
		IrcApplicationLayer(std::string password);
		~IrcApplicationLayer();
		void		connect(int id, struct sockaddr_in address);	
		void		disconnect(int id);
		void		receive(int id, std::string line);
		SendQueue&	getSendQueue();
		int			getUserIdByName(std::string name);
};

#endif
