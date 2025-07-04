#ifndef PENDINGCLIENT_HPP
#define PENDINGCLIENT_HPP

#include "user.hpp"

class PendingClient{
	private:
		std::string username;
		std::string hostname;
		std::string servername;
		std::string realname;
		std::string nickname;
		std::string password;
		int user_fd;

		bool password_set;
		bool nickname_set;
		bool username_set;

		bool username_valid;
		bool nickname_valid;
		bool password_valid;

	public:
		PendingClient(int fd);

		const std::string getUsername() const;
		const std::string getHostname() const;
		const std::string getServername() const;
		const std::string getRealname() const;
		const std::string getNickname() const;
		const std::string getPassword() const;

		int get_fd() const;
		bool get_username_set() const;
		bool get_nickname_set() const;
		bool get_password_set() const;
		bool get_username_valid() const;
		bool get_nickname_valid() const;
		bool get_password_valid() const;

		void setUsername(std::string& username);
		void setHostname(std::string& username);
		void setServername(std::string& username);
		void setRealname(std::string& username);
		void setNickname(std::string& nickname);
		void setPassword(std::string& password);

		bool isNicknameSet() const;
		bool isUsernameSet() const;
		bool isPasswordSet() const;

		bool checkPassword(std::string& password, std::string& truePassword) const;
		bool checkUsername(std::string& username, std::vector <User> users, std::vector <PendingClient> pendingUsers) const;
		bool checkNickname(std::string& nickname, std::vector <User> users, std::vector <PendingClient> pendingUsers) const;

		void handleRegistration(std::string& buff, std::string& password, std::vector <User> users, std::vector <PendingClient> pendingUsers);
		void handleUserCommand(std::string& username, std::vector <User> users, std::vector <PendingClient> pendingUsers);
		void handleNickCommand(std::string& nickname, std::vector <User> users, std::vector <PendingClient> pendingUsers);
		void handlePassCommand(std::string& password, std::string& truePassword);

	};

	std::vector<std::string> splitBySpace(const std::string& input);
	std::string parsse(std::string& std);
	void sendReply(int fd, std::string reply);

#endif
