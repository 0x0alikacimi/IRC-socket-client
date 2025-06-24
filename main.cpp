#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <vector>

class user
{
	private :
	std::string y;
	public :
};

class irc
{
	private :
		std::vector <user> users;
		std::string password;
		int port;
	public :
		irc();
		irc(int port, std::string password);
};

irc::irc(int port, std::string password) : port(port), password(password)
{
	sockaddr_in serv_add;
	std::memset(&serv_add, 0, sizeof(serv_add));
	serv_add.sin_family = AF_INET;//which type of ip (ip4/ ip6)
	serv_add.sin_addr.s_addr = INADDR_ANY;
	serv_add.sin_port = htons(port);
}

int main (int ac , char **av)
{
	if (ac == 3)
	{
		int port = std::atoi(av[1]);
		std::string password = av[2];
		irc server(port, password);
	}
}
