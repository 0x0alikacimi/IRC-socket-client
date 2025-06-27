#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <vector>
#include <fcntl.h>
#include <poll.h>


class User
{
	private :
		std::string name;
		std::string password;
		int client_fd;
	public :
		 User(int fd) : client_fd(fd) {}
		 int get_fd() const {return (client_fd);}
};

class Irc
{
	private :
		int server_fd;
		std::vector <User> users;
		std::string password;
		int port;
	public :
		Irc(int port, std::string password);
		void start_server();
};


void Irc::start_server()
{
	std::cout << "srever is lestenning to port : " << port << std::endl;
	while (true)
	{
		sockaddr_in client_addr ;
		socklen_t client_len = sizeof(client_addr);

		int client_fd = accept(server_fd, (sockaddr *)&client_addr, &client_len);
		if (client_fd < 0)
			continue;
		users.push_back(User(client_fd));
		std::cout << "new client conneeeected !!" << users.size() << std::endl;
	}
}


Irc::Irc(int port, std::string password) : port(port), password(password)
{
	sockaddr_in serv_add;
	std::memset(&serv_add, 0, sizeof(serv_add));
	serv_add.sin_family = AF_INET;//which type of ip (ip4/ ip6)
	serv_add.sin_addr.s_addr = INADDR_ANY;
	serv_add.sin_port = htons(port);

	server_fd = socket(AF_INET, SOCK_STREAM, 0);/*It opens a virtual communication endpoint*/
	//not connected yet or associated with any thing


	bind(server_fd, (sockaddr *)&serv_add, sizeof(serv_add));/*associates the socket with a specific IP address and port number*/
	//(this socket claims a spot on the network) its like “I’m listening on IP X.X.X.X and port Y.”

	listen(server_fd, 1);

}

int main (int ac , char **av)
{
	if (ac == 3)
	{
		/*check if it valid else throw an exeption*/
		int port = std::atoi(av[1]);
		std::string password = av[2];
		Irc server(port, password);
		server.start_server();
	}
	/*throw unvalid number of arguments*/
}
