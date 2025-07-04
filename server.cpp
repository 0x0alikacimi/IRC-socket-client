#include "server.hpp"
#include "replies.hpp"

void Server::start_server()
{
	std::cout << "srever is lestenning to port : " << port << "\n" << std::endl;
	std::vector<pollfd> pfds;

	struct pollfd pfd; pfd.fd = server_fd; pfd.events = POLLIN;pfd.revents = 0;
	pfds.push_back(pfd);
	struct sockaddr_in client_addr;
	socklen_t client_len = sizeof(client_addr);

	while (true)
	{
		int check_poll = poll(pfds.data(), pfds.size(), BLOCK_WAIT);
		if (check_poll < 0)
		{
			std::cerr << "poll " << std::endl;
			break;
		}
		if (pfds[0].revents & POLLIN)
		{
			int new_users_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);/*!*/
			if (new_users_fd < 0)
			{
				std::cerr << "error accepting new client." << std::endl;
				continue;
			}

			struct pollfd new_pfd; new_pfd.fd = new_users_fd; new_pfd.events = POLLIN; new_pfd.revents = 0;
			pfds.push_back(new_pfd);

			pending_users.push_back(new_users_fd);
			std::cout << "New client connected, waiting for authentication..." << std::endl;
		}

		int i = 1;
		while(i < pfds.size())
		{
			if (pfds[i].revents & POLLIN)
			{
				char buff[1024];
				int n = read(pfds[i].fd, buff, sizeof(buff) - 1);
				if (n <= 0)
				{
					close (pfds[i].fd);
					std::cout << "Client disconnected!" << std::endl;
					remove_pending_client(pfds[i].fd);
					remove_user(pfds[i].fd);
					pfds.erase(pfds.begin() + i);
					i--;
				}
				else
				{
					buff[n] = '\0';
					PendingClient *pending = look_for_pending(pfds[i].fd);
					User *user = look_for_user(pfds[i].fd);

					if (pending)
					{
						/*deal with pending*/
						std::string save1 = buff;
						if (isReadyForRegistration(save1, pending))
						{
							std::string msg = "Welcome to the Internet Relay Network " + pending->getNickname() + "!" + pending->getUsername() + "@" + pending->getUsername();
							sendReply(pending->get_fd(), RPL_WELCOME(pending->getUsername(), msg));
							User new_user(pending->get_fd(), pending->getUsername(), pending->getHostname(), pending->getServername(), pending->getRealname(), pending->getNickname());
							users.push_back(new_user);
							remove_pending_client(pending->get_fd());
						}
					}
					else if (user)
					{
						/*deal with user*/
						std::string save2 = buff;
						dealWIthUser(save2, user);
					}

					// std::cout << "received form " << i << ": " << buff << std::endl;
				}
			}
			i++;
		}
	}
	close(server_fd);
}

Server::Server(int port, std::string password) : port(port), password(password)
{
	server_fd = socket(AF_INET, SOCK_STREAM, DEF_PROTOCOL);

	fcntl(server_fd, F_SETFL, O_NONBLOCK);

	sockaddr_in serv_add;
	std::memset(&serv_add, 0, sizeof(serv_add));
	serv_add.sin_family = AF_INET;
	serv_add.sin_addr.s_addr = INADDR_ANY;
	serv_add.sin_port = htons(port);

	bind(server_fd, (sockaddr *)&serv_add, sizeof(serv_add));

	listen(server_fd, MAX_PENDING);
}

std::vector <User> Server::getUsers(){
	return this->users;
}

Channel* Server::getChannel(std::string& name, std::string& key){
	std::vector <Channel>::iterator it = channels.begin();
	for (; it != channels.end(); ++it)
		if (it->getName() == name)
			return &(*it);
	Channel new_channel(name, key);
	channels.push_back(new_channel);
	return &channels.back();
}

void Server::dealWIthUser(std::string& buff, User* user){
	std::vector<std::string> tokens = splitBySpace(buff);
	if (tokens.empty()) {
		std::cout << "empty command" << std::endl;
		return;
	}
	std::string command = tokens[0];
	if (command == "JOIN" && tokens.size() >= 2 && tokens.size() <= 3){
		std::string name = tokens[1];
		std::string key = "";
		if (tokens.size() == 3)
			key = tokens[2];
		Channel* channel = getChannel(name, key);
		channel->handleJoinCommand(user, key);
	}
	else if (command == "KICK" && tokens.size() >= 3 && tokens.size() <= 4){
		std::string channelName = tokens[1];
		std::string name = tokens[2];
		std::string delComment = "";
		if (tokens.size() == 4)
			delComment = tokens[3];
		Channel* channel = getChannelName(channelName);
		if (!channel)
			return ;
		User* delUser = getDelUser(name);
		if (!delUser)
			return ;
		channel->handleKickCommand(user, delUser, delComment);
	}
	else {
		std::cout << "Unknown/Unvalid command" << std::endl;
		return;
	}
}

Channel* Server::getChannelName(std::string& delChannel){
	std::vector <Channel>::iterator it = channels.begin();
	for (; it != channels.end(); ++it)
		if (it->getName() == delChannel)
			return &(*it);
	std::cout << "The Channel : " << delChannel << " does not exist " << std::endl;
	return NULL;
}

User* Server::getDelUser(std::string& name){
	std::vector <User>::iterator it = users.begin();
	for (; it != users.end(); ++it)
		if (it->getUsername() == name)
			return &(*it);
	std::cout << "The User : " << name << " does not exist in the server " << std::endl;
	return NULL;
}

