#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <vector>

class user
{
	private :
		std::string name;
		std::string password;
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
	std::memset(&serv_add, 0, sizeof(serv_add));
	serv_add.sin_family = AF_INET;//which type of ip (ip4/ ip6)
	serv_add.sin_addr.s_addr = INADDR_ANY;
	serv_add.sin_port = htons(port);

	int server_fd = socket(AF_INET, SOCK_STREAM, 0);/*It opens a virtual communication endpoint*/
	//not connected yet or associated with any thing

	bind(server_fd, (sockaddr *)&serv_add, sizeof(serv_add));/*associates the socket with a specific IP address and port number*/
	//its like “I’m listening on IP X.X.X.X and port Y.”

	// listen(server_fd)
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
