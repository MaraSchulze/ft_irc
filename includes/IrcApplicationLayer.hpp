/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IrcApplicationLayer.hpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marschul <marschul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/17 16:39:00 by marschul          #+#    #+#             */
/*   Updated: 2024/05/01 20:25:33 by marschul         ###   ########.fr       */
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
		std::string						_serverCreationTime;

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
		void	handleCap(User& user, std::string line);
		void	handlePing(User& user, std::string line);
		void	sendServerMessage(User& user, std::string code, std::string text);
		void	sendPrefixMessage(User& sender, User& receiver, std::string command, std::string text);
		void	send(int id, std::string message);
		void	sendWelcome(User& user);
		User	*getUser(int id);
		Channel	*getChannel(std::string name);
		void	deleteUser(int id);
		void	deleteChannel(std::string name);
	public:
		IrcApplicationLayer(std::string password);
		~IrcApplicationLayer();
		void		connect(int id, struct sockaddr_in address);	
		void		disconnect(int id);
		void		receive(int id, std::string line);
		SendQueue&	getSendQueue();
		int			getUserIdByName(std::string name);
		void		sendError(User& user, std::string errorcode, std::string errorMessage);
		void		sendPrefixMessageToMany(User& user, std::vector<int> ids, std::string command, std::string text);
};

#endif
