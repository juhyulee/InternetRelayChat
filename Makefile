SERVER = ircserv

SRCS_S = main.cpp Server.cpp Client.cpp

OBJS_S = $(SRCS_S:.cpp=.o)

CXX = c++
FLAGS = -Wall -Wextra -Werror -std=c++98 -g
RM = rm -rf

all : 		$(SERVER)

$(SERVER) :	$(OBJS_S)
			$(CXX) $(FLAGS) -o $(SERVER) $(OBJS_S)

%.o : %.cpp
			$(CXX) $(FLAGS) -c $< -o $@

clean :
			$(RM) $(OBJS_S)

fclean :	clean
			$(RM) $(SERVER)

re :		fclean all

.PHONY : all clean fclean re
