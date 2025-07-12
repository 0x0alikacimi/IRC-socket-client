#ifndef SERVER_HPP
#define SERVER_HPP

#include <ctime>
#include "pendingClient.hpp"
#include "user.hpp"
#include "channel.hpp"

class Server
{
	private :
		int server_fd;
		int port;
		std::vector <User> users;
		std::vector <PendingClient> pending_users;
		std::vector <Channel> channels;
		std::string password;
	public :
		Server(int port, std::string password);
		void start_server();
		User *look_for_user(int fd);
		PendingClient *look_for_pending(int fd);

		std::vector <User> getUsers();

		void remove_pending_client(int fd);
		void remove_user(int fd);
		void pending_to_user(PendingClient *client);

		bool isReadyForRegistration(std::vector<std::string> tokens, PendingClient* pending) const;

		void dealWIthUser(std::string& buff, User* user);
		Channel* getChannel(std::string& name, std::string& key);
		Channel* getChannelName(std::string& delChannel);
		User* getDelUser(std::string& name);
		User* getUserByfd(std::string& name);
		User* getUserByFd(int fd);

		void joinCmd(std::vector<std::string> tokens, User* user);
		void kickCmd(std::vector<std::string> tokens, User* user);
		void inviteCmd(std::vector<std::string> tokens, User* user);
		void topicCmd(std::vector<std::string> tokens, User* user);
		void modeCmd(std::vector<std::string> tokens, User* user);

		void handleMultipleModes(Channel* channel, std::vector<std::string> tokens, User* user, std::string mode);

		void handlePrivateMessage(std::vector<std::string> tokens, std::vector<User> users, std::vector<Channel> &channels, User *sender);
		void to_user(User sender, std::string msg, std::string target, std::vector<User> users);
		void to_channel(User sender, std::string msg,std::string target, std::vector <Channel> cha);
		void deal_with_bot(User sender, std::string msg);
};

#endif













