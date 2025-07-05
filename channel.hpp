#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "user.hpp"

class Channel{
    private:
        std::string name;
        std::string key;
        std::string topic;
		bool justCreated;
		bool inviteOnly;
		bool requiredKey;
		bool topicRest;
		// bool isFull;
		int maxUsers;
		int currentUsers;
        std::vector <int> users_fd;
        std::vector <int> inviteds_fd;
        std::vector <int> operators_fd;

    public:
        Channel(const std::string& name, const std::string& key = "");

        const std::string& getName() const;
        const std::string& getTopic() const;
        const std::string& getKey() const;
		const bool getTopicRest() const ;

        void setTopic(const std::string& topic);
		void setTopicRest();
        void addUser(User* user, const std::string& key);
        void removeUser(int users_fd);
        bool isUserInChannel(int users_fd) const;
        void addOperator(int users_fd);
        bool isOperator(int users_fd) const;
        void invite(int users_fd);
        bool isInvited(int users_fd) const;
		void addInvited(int fd);
        bool hasKey();
        bool checkKey(const std::string& key) const;
		bool isFull() const;

		void handleJoinCommand(User* user, std::string& key);
		void handleKickCommand(User* user, User* delUser, std::string& delComment);
};

#endif
