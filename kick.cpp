#include "channel.hpp"
#include "user.hpp"
#include "replies.hpp"
#include "pendingClient.hpp"

void Channel::removeUser(int user_fd) {
    std::vector<int>::iterator user_vector;
    user_vector = std::find(users_fd.begin(), users_fd.end(), user_fd);
    if (user_vector != users_fd.end())
        users_fd.erase(user_vector);
    user_vector = std::find(operators_fd.begin(), operators_fd.end(), user_fd);
    if (user_vector != operators_fd.end())
        operators_fd.erase(user_vector);
    user_vector = std::find(inviteds_fd.begin(), inviteds_fd.end(), user_fd);
    if (user_vector != inviteds_fd.end())
        inviteds_fd.erase(user_vector);
}

void Channel::handleKickCommand(User* user, User* delUser, std::string& delComment){
	if (!isUserInChannel(user->get_fd())){
		sendReply(user->get_fd(), ERR_NOTONCHANNEL(user->getNickname(), name));
		return ;
	}
	if (!isOperator(user->get_fd())){
		sendReply(user->get_fd(), ERR_CHANOPRIVSNEEDED(name));
		return ;
	}
	if (!isUserInChannel(delUser->get_fd())){
		sendReply(user->get_fd(), ERR_NOTONCHANNEL(user->getNickname(), name));
		return ;
	}
	removeUser(delUser->get_fd());
	sendReply(user->get_fd(), RPL_KICK(user->getNickname(), name, delUser->getNickname(), delComment));
}
