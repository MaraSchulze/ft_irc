/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marschul <marschul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/18 16:35:10 by marschul          #+#    #+#             */
/*   Updated: 2024/04/22 16:22:14 by marschul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <string>
# include <vector>

class  Channel {
	private:
		std::string			_name;
		std::vector<int>	_members;
		std::vector<int>	_operators;
		bool				_modeI;
		bool				_modeK;
		bool				_modeL;
		bool				_modeT;
		std::string			_key;
		int					_userLimit;
		std::string			_topic;
	public:
		Channel(std::string name);
		~Channel();
		void	addMember(int id);
		void	removeMember(int id);
		bool	isMember(int id);
		std::vector<int>&	getMembers();
		void	addOperator(int id);
		void	removeOperator(int id);
		bool	isOperator(int id);
		void	setModeI(bool i);
		bool	getModeI();
		void	setModeK(bool k);
		bool	getModeK();
		void	setModeL(bool l);
		bool	getModeL();
		void	setModeT(bool t);
		bool	getModeT();
		std::string	getKey();
		void	setKey(std::string key);
		void	setUserLimit(int userLimit);
		int		getUserLimit();
		std::string	getTopic();
		void	setTopic(std::string topic);
};

#endif
