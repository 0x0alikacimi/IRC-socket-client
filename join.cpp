#include "channel.hpp"
#include "user.hpp"
#include "replies.hpp"
#include "pendingClient.hpp"

void Channel::handleJoinCommand(User* user, std::string& key){
	addUser(user, key);
}

void Channel::addUser(User* user, const std::string& key){
    if (isUserInChannel(user->get_fd())){
		sendReply(user->get_fd(), ERR_ALREADYREGISTRED(user->getUsername()));
        return ;
	}
	if (hasKey() && !checkKey(key)){
		sendReply(user->get_fd(), ERR_BADCHANNELKEY(user->getUsername(), name));
        return ;
	}
    users_fd.push_back(user->get_fd());
	sendReply(user->get_fd(), RPL_JOIN(user->getUsername(), name));
	sendReply(user->get_fd(), RPL_TOPIC(user->getUsername(), name, topic));
	sendReply(user->get_fd(), RPL_JOINMSG(user->getHostname(), "127.0.0.1", name));
	if (justCreated){
		addOperator(user->get_fd());
		justCreated = false;
	}
}
