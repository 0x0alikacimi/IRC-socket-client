#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <vector>

class user
{
	private :
		int x = 0;
	public :
};

class irc
{
	private :
		std::vector <user> users;
		std::string password;
		int port;
	public :
		irc(int port, std::string password);
};

irc::irc(int port, std::string password) : port(port), password(password)
{
	sockaddr_in serv_add;

}

int main (int ac , char **av)
{
	if (ac == 3)
	{
		/*check if it valid else throw an exeption*/
		int port = std::atoi(av[1]);
		std::string password = av[2];
		irc server(port, password);
	}
	/*throw in valid number of arguments*/
}
