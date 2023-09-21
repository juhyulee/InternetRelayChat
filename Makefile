SERVER = ircserv

SRCS_S = selectserver.cpp channel.cpp server.cpp client.cpp

OBJS_S = $(SRCS_S:.cpp=.o)

CXX = c++
FLAGS = -Wall -Wextra -Werror -std=c++98
RM = rm -rf

all : 		$(SERVER)

$(SERVER) :	$(OBJS_S)
			$(CXX) $(FLAGS) -o $(SERVER) $(OBJS_S)

clean :
			$(RM) $(OBJS_S)

fclean :	clean
			$(RM) $(SERVER)

re :		fclean all

.PHONY : all clean fclean re
