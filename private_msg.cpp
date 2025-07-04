#include "server.hpp"
#include "replies.hpp"

void	send_msg(int fd, std::string  msg)
{
	send(fd, msg.c_str(), msg.size(), 0);
}

std::vector<std::string> get_recs(std::string &receiversToken)
{
	std::vector<std::string> tokens;
	std::string tkn;
	int i = 0;
	while (i < receiversToken.length())
	{
		if (receiversToken[i] == ',')
		{
			if (!tkn.empty())
			{
				tokens.push_back(tkn);
				tkn.clear();
			}
		}
		else
			tkn += receiversToken[i];
		i++;
	}
	if (!tkn.empty())
		tokens.push_back(tkn);
	return (tokens);
}

void handlePrivateMessage(std::vector <std::string> tokens, std::vector<User> users, User *sender)
{
	std::string msg_str;
	if (tokens.size() < 3)
	{
		std::string err_msg = ERR_NEEDMOREPARAMS(tokens[0]);
		send_msg(sender->get_fd(), err_msg);
		return;
	}
	std::vector <std::string> vec_recivers = get_recs(tokens[1]);
	std::string msg;
	if (!tokens[2].empty() && tokens[2][0] == ':')
		msg = tokens[2].substr(1);
	else
		msg = tokens[2];

	int i = 3;
	while (i < tokens.size())
	{
		msg += " " + tokens[i];
		i++;
	}
	std::string target;
	std::vector<User>::iterator it;
	bool valid = false;
	i = 0;
	while (i < vec_recivers.size())
	{
		it = users.begin(); ;
		target = vec_recivers[i];
		valid = false;
		while (it != users.end())
		{
			if (it->getNickname() == target)
			{
				valid = true;
				msg_str = "PRIVMSG " + target + " :" + msg + "\r\n";
				send_msg(it->get_fd(), msg_str);
				break;
			}
			it++;
		}
		if (!valid)
		{
			msg_str = ":server 401 " + sender->getNickname() + " " + target + " :No such nick/channel\r\n";
			send_msg(sender->get_fd(), msg_str);
		}
		i++;
	}
}



