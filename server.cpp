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
			int new_users_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
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
						std::string save1 = pending->getBuffer();
						save1 += buff;
						if (save1.find('\n') != std::string::npos){
							pending->setBufferEmpty();
							std::vector<std::string> tokens = splitByLine(save1);
							for (int i = 0; i < tokens.size(); ++i){
								if (isReadyForRegistration(splitBySpace(tokens[i]), pending))
								{
									std::string msg = "Welcome to the Internet Relay Network " + pending->getNickname() + "!" + pending->getUsername() + "@" + pending->getUsername();
									sendReply(pending->get_fd(), RPL_WELCOME(pending->getUsername(), msg));
									User new_user(pending->get_fd(), pending->getUsername(), pending->getHostname(), pending->getServername(), pending->getRealname(), pending->getNickname());
									users.push_back(new_user);
									remove_pending_client(pending->get_fd());
								}
							}
						}
						else
							pending->setBuffer(buff);
					}
					else if (user)
					{
						std::string save2 = user->getBuffer();
						save2 += buff;
						if (save2.find('\n') != std::string::npos){
							user->setBufferEmpty();
							dealWIthUser(save2, user);
						}
						else
							user->setBuffer(buff);
					}
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
	if (server_fd == -1)
		throw std::runtime_error("Failed to create socket");

	int opt = 1;
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
		throw std::runtime_error("Failed to set socket options");

	fcntl(server_fd, F_SETFL, O_NONBLOCK);

	sockaddr_in serv_add;
	std::memset(&serv_add, 0, sizeof(serv_add));
	serv_add.sin_family = AF_INET;
	serv_add.sin_addr.s_addr = INADDR_ANY;
	serv_add.sin_port = htons(port);

	if (bind(server_fd, (sockaddr *)&serv_add, sizeof(serv_add)))
		throw std::runtime_error("Failed to bind socket");
	if(listen(server_fd, MAX_PENDING))
		throw std::runtime_error("Failed to listen on socket");
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

Channel* Server::getChannelName(std::string& delChannel){
	if (channels.empty())
		return NULL;
	std::vector <Channel>::iterator it = channels.begin();
	for (; it != channels.end(); ++it)
		if (it->getName() == delChannel)
			return &(*it);
	return NULL;
}

User* Server::getDelUser(std::string& name){
	std::vector <User>::iterator it = users.begin();
	for (; it != users.end(); ++it)
		if (it->getNickname() == name)
			return &(*it);
	return NULL;
}

User* Server::getUserByFd(int fd){
	std::vector <User>::iterator it = users.begin();
	for (; it != users.end(); ++it)
		if (it->get_fd() == fd)
			return &(*it);
	return NULL;
}

void call_boot(User *user)
{
	std::time_t t = std::time(NULL);
	std::string str_time = std::ctime(&t);
	std::string rep_msg = RPL_PRIVMSG(user->getNickname(), user->getNickname(), str_time);
	sendReply(user->get_fd(), rep_msg);
}

void Server::dealWIthUser(std::string& buff, User* user){
	std::vector<std::string> tokens = splitBySpace(buff);
	if (tokens.empty())
		return;
	std::string command = tokens[0];
	if (command == "JOIN" && tokens.size() >= 2 && tokens.size() <= 3){
		joinCmd(tokens, user);
	}
	else if (command == "KICK" && tokens.size() <= 4){
		kickCmd(tokens, user);
	}
	else if (command == "INVITE"  && tokens.size() <= 3){
		inviteCmd(tokens, user);
	}
	else if (command == "PRIVMSG"){
		// handlePrivateMessage(tokens, users, user);
	}
	else if (command == "TOPIC"){
		topicCmd(tokens, user);
	}
	else if (command == "MODE"){
		modeCmd(tokens, user);
	}
	else if ((command == "USER" || command == "NICK" || command == "PASS") && tokens.size() == 2){
		sendReply(user->get_fd(), ERR_ALREADYREGISTRED(user->getNickname()));
	}
	else if (command == "USER" && tokens.size() == 2){
		sendReply(user->get_fd(), ERR_ALREADYREGISTRED(user->getNickname()));
	}
	else if (command == "TIME")
	{
		call_boot(user);
	}
	else {
	}
}
