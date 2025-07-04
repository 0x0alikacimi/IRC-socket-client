#include "channel.hpp"
#include "user.hpp"
#include "replies.hpp"
#include "pendingClient.hpp"

void Channel::handleJoinCommand(User* user, std::string& key){
	if (addUser(user->get_fd(), key, user->getUsername()))
		addOperator(user->get_fd());
}

bool Channel::addUser(int user_fd, const std::string& key, const std::string& username){
    if (isUserInChannel(user_fd)){
		sendReply(user_fd, ERR_ALREADYREGISTRED(username));
        return false;
	}
	if (hasKey() && !checkKey(key)){
		sendReply(user_fd, ERR_BADCHANNELKEY(username, name));
        return false;
	}
    users_fd.push_back(user_fd);
	std::cout << "The user added successfully to Channel : " << name << std::endl;
	return true;
}
