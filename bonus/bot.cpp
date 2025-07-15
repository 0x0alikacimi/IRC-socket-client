#include <arpa/inet.h>
#include <iostream>
#include <unistd.h>
#include <ctime>
#include <sstream>
#include <sys/socket.h>

#define POSTFIX "\r\n"
#define RPL_PRIVMSG(sender, target, msg) ":" + sender + " PRIVMSG " + target + " :" + msg + POSTFIX

void	send_msg(int fd, std::string  msg)
{
	send(fd, msg.c_str(), msg.size(), 0);
}

void send_registration(int fd, std::string &password, std::string &nick, std::string &username)
{
	std::string pass_cmd = "PASS " + password + "\r\n";
	std::string nick_cmd = "NICK " + nick + "\r\n";
	// USER <username> <hostname> <servername> :<realname>
	std::string user_cmd = "USER " + username + " 0 * :" + username + "\r\n";

	send(fd, pass_cmd.c_str(), pass_cmd.size(), 0);
	send(fd, nick_cmd.c_str(), nick_cmd.size(), 0);
	send(fd, user_cmd.c_str(), user_cmd.size(), 0);
}

void call_bot(std::string buff, int cl_fd)
{
	std::time_t t = std::time(NULL);
	std::string str_time = std::ctime(&t);
	std::string name = buff;
	std::string bot_name = "bot";
	std::string rep = "PRIVMSG " + name + " " + str_time;
	send_msg(cl_fd, rep);
}

int main(int ac, char **av)
{
	if (ac == 3)
	{
		std::string pass = av[2];
		int port = std::atoi(av[1]);
		if (port < 1024 || port > 49151)
		{
			std::cerr << "Invalid port: must be between 1024 and 65535." << std::endl;
			return 1;
		}
		int client_fd = socket(AF_INET, SOCK_STREAM, 0);
		if (client_fd < 0)
		{
			std::cerr << "socket failed" << std::endl;
			return 1;
		}
		sockaddr_in server_add;
		std::memset(&server_add, 0, sizeof(server_add));
		server_add.sin_family = AF_INET;
		server_add.sin_port = htons(port);
		server_add.sin_addr.s_addr = inet_addr("127.0.0.1");/**/
		if (connect(client_fd, (sockaddr *)&server_add, sizeof(server_add)) < 0)
		{
			std::cerr << "connect failed" << std::endl;
			close(client_fd);
			return 1;
		}
		std::string bot_name = "bot";
		send_registration(client_fd, pass, bot_name, bot_name);
		std::string msg;

		while(true)
		{
			char buffer[1024];
			int bytes;
			while ((bytes = read(client_fd, buffer, sizeof(buffer) - 1)) > 0)
			{
				buffer[bytes] = '\0';
				call_bot(buffer, client_fd);
			}
			if (bytes <= 0)
			{
				std::cout << "Server closed connection" << std::endl;
				break;
			}
		}
		close(client_fd);
	}
	else
	{
		std::cerr << "Usage: " << av[0] << " <port> <password>" << std::endl;
	}
}
