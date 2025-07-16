#include "server.hpp"

<<<<<<< HEAD
#define DEF_PROTOCOL 0 /*the default protocol for IPv4 TCP, which is TCP itself.*/
#define MAX_PENDING 1 /*is the backlog queue size (max pending connections waiting).*/
#define BLOCK_WAIT -1 /*blocks indefinitely until at least one socket has some activity*/

class User
=======
bool valid(std::string str)
>>>>>>> master
{
	size_t i = 0;
	while (str[i])
	{
		if (str[i] == ' ' || str[i] == '	')
			return (false);
		i++;
	}
	return (true);
}

<<<<<<< HEAD
void Irc::start_server()
=======

int main (int ac , char **av)
>>>>>>> master
{
	if (ac == 3)
	{
<<<<<<< HEAD
		int check_poll = poll(pfds.data(), pfds.size(), BLOCK_WAIT);
		if (check_poll < 0)
		{
			std::cerr << "pll " << std::endl;
			break;
		}
		if (pfds[0].revents & POLLIN)
=======
		try
>>>>>>> master
		{
			signal(SIGQUIT, Server::sig_handler);
			signal(SIGINT, Server::sig_handler);
			signal(SIGPIPE, SIG_IGN);
			int port = std::atoi(av[1]);
			if (port < 1024 || port > 49151)
			{
				std::cerr << "invalid port number" << std::endl;
				return 1;
			}
<<<<<<< HEAD
			/*fcntl(new_client_fd, F_SETFL, O_NONBLOCK);*/
			struct pollfd new_pfd; new_pfd.fd = new_client_fd; new_pfd.events = POLLIN; new_pfd.revents = 0;
			pfds.push_back(new_pfd);
			/*now I gotta check the password and the rest of data before accepting him or not */
			users.push_back(User(new_client_fd));
			std::cout << "new client connected!" << "\n" << std::endl;
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
					pfds.erase(pfds.begin() + i);
					users.erase(users.begin() + (i - 1));
					i--;
				}
				else
				{
					buff[n] = '\0';

					std::cout << "received form " << i << ": " << buff << std::endl;
				}
=======
			std::string password = av[2];
			if (password.size() < 3 || !valid(password))
			{
				std::cerr << "invalid password " << std::endl;
				return 1;
>>>>>>> master
			}
			Server server(port, password);
			server.start_server();
		}
		catch(std::exception &e)
		{
			std::cerr << e.what() << std::endl;
			return (1);
		}
	}
<<<<<<< HEAD
	close(server_fd);
}

Irc::Irc(int port, std::string password) : port(port), password(password)
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

int main (int ac , char **av)
{
	if (ac == 3)
	{
		try
		{
			int port = std::atoi(av[1]);
			if (port <= 0 || port >= 65535)
			{
				std::cerr << "invalid port number" << std::endl;
				return 1;
			}
			std::string password = av[2];
			if (password.size() < 3)
			{
				std::cerr << "invalid password " << std::endl;
				return 1;
			}
			Irc server(port, password);
			server.start_server();
		}
		catch(std::exception &e)
		{
			std::cerr << e.what() << std::endl;
			return (1);
		}
		return (0);
=======
	else
	{
		std::cerr << "Usage: " << av[0] << " <port> <password>" << std::endl;
>>>>>>> master
	}
}
