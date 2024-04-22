/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SendQueue.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marschul <marschul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/22 16:34:27 by marschul          #+#    #+#             */
/*   Updated: 2024/04/22 16:42:07 by marschul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SENDQUEUE_HPP
# define SENDQUEUE_HPP

# include <queue>
# include <string>

class  SendQueue {
	private:
		std::queue<int>			_id;
		std::queue<std::string>	_message;
	public:
		SendQueue();
		~SendQueue();
		void		push(int id, std::string message);
		void		pop();
		int			getNextSender();
		std::string	getNextMessage();
		bool		empty();
		size_t		size();
};

#endif
