/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marschul <marschul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/18 16:35:10 by marschul          #+#    #+#             */
/*   Updated: 2024/04/18 22:34:59 by marschul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <string>
# include <vector>

class  Channel {
	private:
		std::string			_name;
		std::string			_topic;
		std::vector<int>	_members;
	public:
		Channel(std::string name);
		~Channel();
		void	addMember(int id);
		std::vector<int>&	getMembers();
		void	removeMember(int id);
};

#endif
