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
		std::cout << "The User : " << user->getUsername() << " is NOT Allowed to KICK from this channel" << std::endl;
		// sendReply(user_fd, ERR_ALREADYREGISTRED(user->getUsername()));
		return ;
	}
	if (!isOperator(user->get_fd())){
		std::cout << "The User : " << user->getUsername() << " is not an operator " << std::endl;
		return ;
	}
	if (!isUserInChannel(delUser->get_fd())){
		std::cout << "The User : " << delUser->getUsername() << " is not in channel" << std::endl;
		return ;
	}
	removeUser(delUser->get_fd());
	std::cout << "The User : " << delUser->getUsername() << " has been removed successfully " ;
	if (!delComment.empty())
		std::cout << "Because : " << delComment << std::endl;
	else
		std::cout << std::endl;
}
