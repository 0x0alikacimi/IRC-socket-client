#include "pendingClient.hpp"
#include "server.hpp"
#include "replies.hpp"

bool Server::isReadyForRegistration(std::string& buff, PendingClient* pending) const{
	std::string pwd = password;
	pending->handleRegistration(buff, pwd, users, pending_users);
	if (pending->get_username_set() && pending->get_nickname_set() && pending->get_password_set()  && pending->get_nickname_valid() && pending->get_password_valid() && pending->get_hostname_set() && pending->get_servername_set() && pending->get_realname_set())
		return true;
	return false;
}

void PendingClient::handleRegistration(std::string& buff, std::string& password, std::vector <User> users, std::vector <PendingClient> pendingUsers){
	std::vector<std::string> tokens = splitBySpace(buff);
	if (tokens.empty())
		return;
	std::string command = parsse(tokens[0]);
	if (command != "NICK" && command != "PASS" && command != "USER"){
		sendReply(user_fd, ERR_UNKNOWNCOMMAND(command));
		return ;
	}
	if (tokens.size() < 2){
		sendReply(user_fd, ERR_NEEDMOREPARAMS(command));
		return ;
	}
	std::string value = parsse(tokens[1]);
	if (command == "NICK" && tokens.size() == 2) {
		handleNickCommand(value, users, pendingUsers);
	}
	else if (command == "PASS" && tokens.size() == 2) {
		handlePassCommand(value, password);
	}
	else if (command == "USER" && tokens.size() <= 5) {
		if (tokens.size() == 5){
			std::string value1 = parsse(tokens[2]);
			std::string value2 = parsse(tokens[3]);
			std::string value3 = parsse(tokens[4]);
			handleUserCommand(value, value1, value2, value3, users, pendingUsers);
		}
		else
			sendReply(user_fd, ERR_NEEDMOREPARAMS(command));
	}
	else if (command == "JOIN" || command == "KICK" || command == "PRIVMSG" || command == "MODE" || command == "TOPIC" || command == "INVITE"){
		sendReply(user_fd, ERR_NOTREGISTERED);
	}
	else {
		sendReply(user_fd, ERR_UNKNOWNCOMMAND(command));
	}
}

void PendingClient::handleUserCommand(std::string& username, std::string& hostname, std::string& servername, std::string& realname, std::vector <User> users, std::vector <PendingClient> pendingUsers){
	setUsername(username);
	setHostname(hostname);
	setServername(servername);
	setRealname(realname);
	if (isUsernameSet())
		this->username_set = true;
	if (isHostnameSet())
		this->hostname_set = true;
	if (isServernameSet())
		this->servername_set = true;
	if (isRealnameSet())
		this->realname_set = true;
}

void PendingClient::handleNickCommand(std::string& nickname, std::vector <User> users, std::vector <PendingClient> pendingUsers){
	setNickname(nickname);
	if (isNicknameSet())
		this->nickname_set = true;
	if (checkNickname(nickname, users, pendingUsers))
		this->nickname_valid = true;
}

void PendingClient::handlePassCommand(std::string& password, std::string& truePassword){
	setPassword(password);
	if (isPasswordSet())
		this->password_set = true;
	if (checkPassword(password, truePassword))
		this->password_valid = true;
}
