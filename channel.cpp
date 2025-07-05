#include "channel.hpp"
#include "user.hpp"
#include "replies.hpp"
#include "pendingClient.hpp"

Channel::Channel(const std::string& name, const std::string& key) : name(name) , key(key), topic(""), justCreated(true), inviteOnly(false), requiredKey(false), maxUsers(MAX_GLOBAL_USERS), currentUsers(0), topicRest(false){
}

const std::string& Channel::getName() const{
    return this->name;
}

const std::string& Channel::getTopic() const{
    return this->topic;
}

const std::string& Channel::getKey() const{
    return this->key;
}
void Channel::setTopic(const std::string& topic){
    this->topic = topic;
}

bool Channel::hasKey() {
    if (!this->key.empty()){
		requiredKey = true;
		return true;
	}
	requiredKey = false;
	return false;
}

bool Channel::checkKey(const std::string& key) const{
    return this->key == key;
}

bool Channel::isUserInChannel(int user_fd) const{
    return std::find(users_fd.begin(), users_fd.end(), user_fd) != users_fd.end();
}

bool Channel::isOperator(int user_fd) const {
    return std::find(operators_fd.begin(), operators_fd.end(), user_fd) != operators_fd.end();
}

void Channel::addOperator(int user_fd){
    if (!isOperator(user_fd)){
        operators_fd.push_back(user_fd);
		return ;
	}
}

bool Channel::isInvited(int user_fd) const{
    return std::find(inviteds_fd.begin(), inviteds_fd.end(), user_fd) != inviteds_fd.end();
}

void Channel::invite(int user_fd){
    if (!isInvited(user_fd))
        inviteds_fd.push_back(user_fd);
}

bool Channel::isFull() const{
	return currentUsers >= maxUsers;
}

void Channel::addInvited(int fd){
	if (!isInvited(fd)){
		inviteds_fd.push_back(fd);
	}
}

const bool Channel::getTopicRest() const {
	return topicRest;
}

void Channel::setTopicRest()  {
	topicRest = true;
}
