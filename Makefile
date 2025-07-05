NAME = ircserv

SRC_FILES = user.cpp helpers.cpp main.cpp pendingClient.cpp server.cpp channel.cpp join.cpp kick.cpp authetification.cpp invite.cpp topic.cpp private_msg.cpp
OBJ_FILES = $(SRC_FILES:.cpp=.o)

HEADERS = server.hpp user.hpp pendingClient.cpp channel.hpp

CC = c++
# CC = g++-15 #for linux
CFLAGS = -fsanitize=address -g #-Wall -Wextra -Werror -std=c++98

all : $(NAME)

%.o: %.cpp $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

$(NAME) : $(OBJ_FILES)
	$(CC) $(CFLAGS) $(OBJ_FILES) -o $(NAME)

clean :
	rm -f $(OBJ_FILES)

fclean : clean
	rm -f $(NAME)

re: fclean all

.PHONY: clean
