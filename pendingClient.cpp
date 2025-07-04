#include "pendingClient.hpp"
#include "server.hpp"
#include "replies.hpp"

PendingClient::PendingClient(int fd){
	this->user_fd = fd;
	this->nickname = "";
	this->password = "";
	this->username = "";
	this->nickname_set = false;
	this->username_set = false;
	this->password_set = false;
	this->nickname_valid = false;
	this->password_valid = false;
}

bool Server::isReadyForRegistration(std::string& buff, PendingClient* pending) const{
	std::string pwd = password;
	pending->handleRegistration(buff, pwd, users, pending_users);
	if (pending->get_username_set() && pending->get_nickname_set() && pending->get_password_set()  && pending->get_nickname_valid() && pending->get_password_valid() && pending->get_hostname_set() && pending->get_servername_set() && pending->get_realname_set())
		return true;
	return false;
}

void PendingClient::handleRegistration(std::string& buff, std::string& password, std::vector <User> users, std::vector <PendingClient> pendingUsers){
	std::vector<std::string> tokens = splitBySpace(buff);
	if (tokens.empty() && tokens.size() < 2){
		// std::cout << "Empty command" << std::endl;
		return;
	}
	std::string command = parsse(tokens[0]);
	std::string value = parsse(tokens[1]);
	if (command == "NICK" && tokens.size() <= 2) {
		if (tokens.size() == 2){
			std::string value = parsse(tokens[1]);
			handleNickCommand(value, users, pendingUsers);
		}
		else
			sendReply(user_fd, ERR_NEEDMOREPARAMS(buff));
	}
	else if (command == "PASS" && tokens.size() <= 2) {
		if (tokens.size() == 2)
			handlePassCommand(value, password);
		else
			sendReply(user_fd, ERR_NEEDMOREPARAMS(buff));
	}
	else if (command == "USER" && tokens.size() <= 5) {
		if (tokens.size() == 5){
			std::string value1 = parsse(tokens[2]);
			std::string value2 = parsse(tokens[3]);
			std::string value3 = parsse(tokens[4]);
			handleUserCommand(value, value1, value2, value3, users, pendingUsers);
		}
		else
			sendReply(user_fd, ERR_NEEDMOREPARAMS(buff));
	}
	else {
		// std::cout << "Unknown command: " << std::endl;
		sendReply(user_fd, ERR_UNKNOWNCOMMAND(buff));
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

bool PendingClient::isUsernameSet() const{
	if (getUsername() != "")
		return true;
	return false;
}

bool PendingClient::isHostnameSet() const{
	if (getHostname() != "")
		return true;
	return false;
}

bool PendingClient::isServernameSet() const{
	if (getServername() != "")
		return true;
	return false;
}

bool PendingClient::isRealnameSet() const{
	if (getRealname() != "")
		return true;
	return false;
}

bool PendingClient::isNicknameSet() const{
	if (getNickname() != "")
		return true;
	return false;
}

bool PendingClient::isPasswordSet() const{
	if (getPassword() != "")
		return true;
	return false;
}

bool PendingClient::checkPassword(std::string& password, std::string& truePassword) const{
	if (truePassword == parsse(password)){
		std::cout << "Password correct" << std::endl;
		return true;
	}
	std::cout << "(" << password << ")" << std::endl;
	std::cout << "Password incorrect" << std::endl;
	sendReply(user_fd, ERR_PASSWDMISMATCH(password));
	return false;
}


bool PendingClient::checkNickname(std::string& nickname, std::vector <User> users, std::vector <PendingClient> pendingUsers) const{
	std::vector<User>::const_iterator it = users.begin();
    for (; it != users.end(); ++it) {
        if (it->getNickname() == nickname) {
			// std::cout << "The nickname " << nickname << " is already in use" << std::endl;
			sendReply(user_fd, ERR_NICKNAMEINUSE(nickname));
            return false;
        }
    }
	std::vector<PendingClient>::const_iterator itt = pendingUsers.begin();
    for (; itt != pendingUsers.end(); ++itt) {
        if (itt->getNickname() == nickname) {
			// std::cout << "The nickname " << nickname << " is already in use" << std::endl;
			sendReply(user_fd, ERR_NICKNAMEINUSE(nickname));
            return false;
        }
    }
	// std::cout << "Nickname set successfully" << std::endl;
    return true;
}

int PendingClient::get_fd() const
{
	return user_fd;
}

const std::string PendingClient::getNickname() const{
	return nickname;
}

const std::string PendingClient::getUsername() const{
	return username;
}

const std::string PendingClient::getHostname() const{
	return hostname;
}

const std::string PendingClient::getServername() const{
	return servername;
}

const std::string PendingClient::getRealname() const{
	return realname;
}

const std::string PendingClient::getPassword() const{
	return password;
}

void PendingClient::setUsername(std::string& username){
	this->username = (username);
}

void PendingClient::setNickname(std::string& nickname){
	this->nickname = (nickname);
}

void PendingClient::setHostname(std::string& hostname){
	this->hostname = (hostname);
}

void PendingClient::setServername(std::string& servername){
	this->servername = (servername);
}

void PendingClient::setRealname(std::string& realname){
	this->realname = realname;
}

void PendingClient::setPassword(std::string& password){
	this->password = password;
}

bool PendingClient::get_username_set()const{
	return username_set;
}
bool PendingClient::get_nickname_set()const{
	return nickname_set;
}
bool PendingClient::get_password_set()const{
	return password_set;
}

bool PendingClient::get_nickname_valid()const{
	return nickname_valid;
}
bool PendingClient::get_password_valid()const{
	return password_valid;
}

bool PendingClient::get_hostname_set() const{
	return hostname_set;
}

bool PendingClient::get_servername_set() const{
	return servername_set;
}

bool PendingClient::get_realname_set() const{
	return realname_set;
}

std::vector<std::string> splitBySpace(const std::string& input) {
	std::vector<std::string> tokens;
	std::string token;
	for (size_t i = 0; i < input.length(); ++i) {
		if (input[i] == ' ') {
			if (!token.empty()) {
				tokens.push_back(token);
				token.clear();
			}
		} else {
			token += input[i];
		}
	}
	if (!token.empty()) {
		tokens.push_back(token);
	}
	return tokens;
}

void sendReply(int fd, std::string reply) {
    send(fd, reply.c_str(), strlen(reply.c_str()), 0);
}

std::string parsse(std::string& str) {
	if (!str.empty() && str.back() == '\n') {
		str.erase(str.size() - 1);
	}
	if (!str.empty() && str.back() == '\r') {
		str.erase(str.size() - 1);
	}
	return str;
}


