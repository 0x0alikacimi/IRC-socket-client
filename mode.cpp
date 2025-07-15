#include "user.hpp"
#include "channel.hpp"
#include "replies.hpp"
#include "pendingClient.hpp"
#include "server.hpp"

bool checkMode(std::string& mode, std::vector <std::string> tokens, int user_fd, std::string name){
	size_t curr = 0;
	if (mode[0] == '+'){
		for (size_t i = 1; i < mode.size(); i++){
			if (mode[i] == 'o' || mode[i] == 'k' || mode[i] == 'l')
				curr++;
			else if (mode[i] != 'i' && mode[i] != 't'){
				sendReply(user_fd, ERR_INVALIDMODEPARM(name, mode));
				return false;
			}
		}
		if (curr > 3){
			sendReply(user_fd, ERR_INVALIDMODEPARM(name, mode));
			return false;
		}
		if (tokens.size() < curr + 3){
			sendReply(user_fd, ERR_INVALIDMODEPARM(name, mode));
			return false;
		}
		if (tokens.size() > curr + 3){
			sendReply(user_fd, ERR_INVALIDMODEPARM(name, mode));
			return false;
		}
		return true;
	}
	else if (mode[0] == '-'){
		for (size_t i = 1; i < mode.size(); i++){
			if (mode[i] == 'o')
				curr++;
			else if (mode[i] != 'i' && mode[i] != 't' && mode[i] != 'k' && mode[i] != 'l'){
				sendReply(user_fd, ERR_INVALIDMODEPARM(name, mode));
				return false;
			}
		}
		if (curr > 3){
			sendReply(user_fd, ERR_INVALIDMODEPARM(name, mode));
			return false;
		}
		if (tokens.size() < curr + 3){
			sendReply(user_fd, ERR_NEEDMODEPARM(name, mode));
			return false;
		}
		if (tokens.size() > curr + 3){
			sendReply(user_fd, ERR_INVALIDMODEPARM(name, mode));
			return false;
		}
		return true;
	}
	else {
		sendReply(user_fd, ERR_INVALIDMODEPARM(name, mode));
		return false;
	}
}

void Server::modeCmd(std::vector<std::string> tokens, User* user){
	std::string command = parsse(tokens[0]);
	if (tokens.size() <= 2){
		sendReply(user->get_fd(), ERR_NEEDMOREPARAMS(command));
		return ;
	}
	Channel* channel = getChannelName(tokens[1]);
	if (!channel){
		sendReply(user->get_fd() ,ERR_NOSUCHCHANNEL(parsse(tokens[1])));
		return ;
	}
	if (!channel->isUserInChannel(user->get_fd())){
		sendReply(user->get_fd() ,ERR_NOTONCHANNEL(user->getNickname(), channel->getName()));
		return ;
	}
	if (!channel->isOperator(user->get_fd())){
		sendReply(user->get_fd() ,ERR_CHANOPRIVSNEEDED(channel->getName()));
		return ;
	}
	std::string mode = parsse(tokens[2]);
	if (mode.size() > 2){
		handleMultipleModes(channel, tokens, user, mode);
		return ;
	}
	if (mode.size() == 2){
		User* opUser = NULL;
		if (mode[1] == 'o'){
			if (tokens.size() != 4){
				sendReply(user->get_fd(), ERR_NEEDMOREPARAMS(command));
				return ;
			}
			opUser = getDelUser(parsse(tokens[3]));
			if (!opUser){
				sendReply(user->get_fd() ,ERR_NOSUCHNICK(parsse(tokens[3])));
				return ;
			}
		}
		channel->handleSingleMode(tokens, user, opUser, mode);
		return ;
	}
	sendReply(user->get_fd() ,ERR_UNKNOWNMODE(std::string(""), channel->getName() , std::string("")));
}

void Channel::handleSingleMode(std::vector<std::string> tokens, User* user, User* opUser, std::string mode){
	if (mode[1] == 'i'){
		handleInviteMode(tokens, user, mode);
	}
	else if (mode[1] == 't'){
		handleTopicMode(tokens, user, mode);
	}
	else if (mode[1] == 'l'){
		handleLimitMode(tokens, user, mode);
	}
	else if (mode[1] == 'k'){
		handleKeyMode(tokens, user, mode);
	}
	else if (mode[1] == 'o'){
		if (opUser)
			handleOperatorMode(tokens, user, opUser, mode);
	}
	else {
		sendReply(user->get_fd() ,ERR_UNKNOWNMODE(std::string(""), name , mode));
	}
}

void Channel::handleInviteMode(std::vector<std::string> tokens, User* user, std::string mode){
	if (tokens.size() != 3){
		sendReply(user->get_fd(), ERR_INVALIDMODEPARM(name, mode));
		return ;
	}
	if (mode[0] == '+'){
		inviteOnly = true;
		for (std::vector<int>::const_iterator it = users_fd.begin(); it != users_fd.end(); ++it){
			sendReply(*it, RPL_UMODEIS(std::string(""), name, "+i", std::string("")));
		}
	}
	else if (mode[0] == '-'){
		inviteOnly = false;
		for (std::vector<int>::const_iterator it = users_fd.begin(); it != users_fd.end(); ++it){
			sendReply(*it, RPL_UMODEIS(std::string(""), name, "-i", std::string("")));
		}
	}
	else
		sendReply(user->get_fd(), ERR_INVALIDMODEPARM(name, mode));
}

void Channel::handleTopicMode(std::vector<std::string> tokens, User* user,  std::string mode){
	if (tokens.size() != 3){
		sendReply(user->get_fd(), ERR_INVALIDMODEPARM(name, mode));
		return ;
	}
	if (mode[0] == '+'){
		topicRest = true;
		for (std::vector<int>::const_iterator it = users_fd.begin(); it != users_fd.end(); ++it){
			sendReply(*it, RPL_UMODEIS(std::string(""), name, "+t", std::string("")));
		}
	}
	else if (mode[0] == '-'){
		topicRest = false;
		for (std::vector<int>::const_iterator it = users_fd.begin(); it != users_fd.end(); ++it){
			sendReply(*it, RPL_UMODEIS(std::string(""), name, "-t", std::string("")));
		}
	}
	else
		sendReply(user->get_fd(), ERR_INVALIDMODEPARM(name, mode));
}

void Channel::handleLimitMode(std::vector<std::string> tokens, User* user,  std::string mode){
	if (mode[0] == '+' && tokens.size() == 4 && atoi(parsse(tokens[3]).c_str())){
		maxUsers = atoi(parsse(tokens[3]).c_str());
		for (std::vector<int>::const_iterator it = users_fd.begin(); it != users_fd.end(); ++it){
			sendReply(*it, RPL_UMODEIS(std::string(""), name, "+l", tokens[3]));
		}
	}
	else if (mode[0] == '-' && tokens.size() == 3){
		maxUsers = MAX_GLOBAL_USERS;
		for (std::vector<int>::const_iterator it = users_fd.begin(); it != users_fd.end(); ++it){
			sendReply(*it, RPL_UMODEIS(std::string(""), name, "-l", std::string("")));
		}
	}
	else
		sendReply(user->get_fd(), ERR_INVALIDMODEPARM(name, mode));
}

void Channel::handleKeyMode(std::vector<std::string> tokens, User* user,  std::string mode){
	if (mode[0] == '+' && tokens.size() == 4){
		key = parsse(tokens[3]);
		for (std::vector<int>::const_iterator it = users_fd.begin(); it != users_fd.end(); ++it){
			sendReply(*it, RPL_UMODEIS(std::string(""), name, "+k", key));
		}
	}
	else if (mode[0] == '-' && tokens.size() == 3){
		key = std::string("");
		for (std::vector<int>::const_iterator it = users_fd.begin(); it != users_fd.end(); ++it){
			sendReply(*it, RPL_UMODEIS(std::string(""), name, "-k", std::string("")));
		}
	}
	else
		sendReply(user->get_fd(), ERR_INVALIDMODEPARM(name, mode));
}

void Channel::handleOperatorMode(std::vector<std::string> tokens, User* user, User* opUser, std::string mode){
	if (tokens.size() != 4){
		sendReply(user->get_fd(), ERR_INVALIDMODEPARM(name, mode));
		return ;
	}
	if (mode[0] == '+' && tokens.size() == 4){
		if (!isUserInChannel(opUser->get_fd())){
			sendReply(user->get_fd() ,ERR_USERNOTINCHANNEL(opUser->getNickname(), name));
			return ;
		}
		addOperator(opUser->get_fd());
		for (std::vector<int>::const_iterator it = users_fd.begin(); it != users_fd.end(); ++it){
			sendReply(*it, RPL_UMODEIS(opUser->getHostname(), name, "+o", opUser->getNickname()));
		}
	}
	else if (mode[0] == '-' && tokens.size() == 4){
		removeOperator(opUser->get_fd());
		for (std::vector<int>::const_iterator it = users_fd.begin(); it != users_fd.end(); ++it){
			sendReply(*it, RPL_UMODEIS(opUser->getHostname(), name, "-o", user->getNickname()));
		}
	}
	else {
		sendReply(user->get_fd(), ERR_INVALIDMODEPARM(name, mode));
	}
}

void Server::handleMultipleModes(Channel* channel, std::vector<std::string> tokens, User* user, std::string mode){
	if (!checkMode(mode, tokens, user->get_fd(), channel->getName()))
		return ;
	int curr = 0;
	if (mode[0] == '+'){
		for (size_t i = 1; i < mode.size(); i++){
			if (mode[i] == 'i'){
				channel->setInviteOnly(true);
				for (std::vector<int>::const_iterator it = (channel->getUsers_fd()).begin(); it != (channel->getUsers_fd()).end(); ++it){
					sendReply(*it, RPL_UMODEIS(std::string(""), channel->getName(), "+i", std::string("")));
				}
			}
			else if (mode[i] == 't'){
				channel->setTopicRest(true);
				for (std::vector<int>::const_iterator it = (channel->getUsers_fd()).begin(); it != (channel->getUsers_fd()).end(); ++it){
					sendReply(*it, RPL_UMODEIS(std::string(""), channel->getName(), "+t", std::string("")));
				}
			}
			else if (mode[i] == 'l'){
				channel->setMaxUsers(atoi(parsse(tokens[3 + curr]).c_str()));
				for (std::vector<int>::const_iterator it = (channel->getUsers_fd()).begin(); it != (channel->getUsers_fd()).end(); ++it){
					sendReply(*it, RPL_UMODEIS(std::string(""), channel->getName(), "+l", tokens[3 + curr]));

				}
				curr++;
			}
			else if (mode[i] == 'k'){
				channel->setKey(parsse(tokens[3 + curr]));
				for (std::vector<int>::const_iterator it = (channel->getUsers_fd()).begin(); it != (channel->getUsers_fd()).end(); ++it){
					sendReply(*it, RPL_UMODEIS(std::string(""), channel->getName(), "+k", channel->getKey()));
				}
				curr++;
			}
			else if (mode[i] == 'o'){
				User* opUser = getDelUser(parsse(tokens[3 + curr]));
				if (!opUser){
					sendReply(user->get_fd() ,ERR_NOSUCHNICK(parsse(tokens[3 + curr])));
					continue ;
				}
				else if (!channel->isUserInChannel(opUser->get_fd())){
					sendReply(user->get_fd() ,ERR_USERNOTINCHANNEL(opUser->getNickname(), channel->getName()));
					continue ;
				}
				channel->addOperator(opUser->get_fd());
				for (std::vector<int>::const_iterator it = (channel->getUsers_fd()).begin(); it != (channel->getUsers_fd()).end(); ++it){
					sendReply(*it, RPL_UMODEIS(opUser->getHostname(), channel->getName(), "+o", opUser->getNickname()));
				}
				curr++;
			}
		}
	}
	else if (mode[0] == '-'){
		for (size_t i = 1; i < mode.size(); i++){
			if (mode[i] == 'i'){
				channel->setInviteOnly(false);
				for (std::vector<int>::const_iterator it = (channel->getUsers_fd()).begin(); it != (channel->getUsers_fd()).end(); ++it){
					sendReply(*it, RPL_UMODEIS(std::string(""), channel->getName(), "-i", std::string("")));
				}
			}
			else if (mode[i] == 't'){
				channel->setTopicRest(false);
				for (std::vector<int>::const_iterator it = (channel->getUsers_fd()).begin(); it != (channel->getUsers_fd()).end(); ++it){
					sendReply(*it, RPL_UMODEIS(std::string(""), channel->getName(), "-t", std::string("")));
				}
			}
			else if (mode[i] == 'l'){
				channel->setMaxUsers(MAX_GLOBAL_USERS);
				for (std::vector<int>::const_iterator it = (channel->getUsers_fd()).begin(); it != (channel->getUsers_fd()).end(); ++it){
					sendReply(*it, RPL_UMODEIS(std::string(""), channel->getName(), "-l", std::string("")));
				}
			}
			else if (mode[i] == 'k'){
				std::string key = "";
				channel->setKey(key);
				for (std::vector<int>::const_iterator it = (channel->getUsers_fd()).begin(); it != (channel->getUsers_fd()).end(); ++it){
					sendReply(*it, RPL_UMODEIS(std::string(""), channel->getName(), "-k",  std::string("")));
				}
			}
			else if (mode[i] == 'o'){
				User* opUser = getDelUser(parsse(tokens[3 + curr]));
				if (!opUser){
					sendReply(user->get_fd() ,ERR_NOSUCHNICK(parsse(tokens[3 + curr])));
					continue ;
				}
				else if (!channel->isUserInChannel(opUser->get_fd())){
					sendReply(user->get_fd() ,ERR_USERNOTINCHANNEL(opUser->getNickname(), channel->getName()));
					continue ;
				}
				channel->removeOperator(opUser->get_fd());
				for (std::vector<int>::const_iterator it = (channel->getUsers_fd()).begin(); it != (channel->getUsers_fd()).end(); ++it){
					sendReply(*it, RPL_UMODEIS(opUser->getHostname(), channel->getName(), "-o", user->getNickname()));
				}
				curr++;
			}
		}
	}
}
