SERVER = server
CLIENT = client

SRCS_S = selectserver.cpp
SRCS_C = echoclient.cpp

OBJS_S = $(SRCS_S:.cpp=.o)
OBJS_C = $(SRCS_C:.cpp=.o)

CXX = c++
FLAGS = -Wall -Wextra -Werror -std=c++98
RM = rm -rf

all : 		$(SERVER) $(CLIENT)

$(SERVER) :	$(OBJS_S)
			$(CXX) $(FLAGS) -o $(SERVER) $(OBJS_S)

$(CLIENT) :	$(OBJS_C)
			$(CXX) $(FLAGS) -o $(CLIENT) $(OBJS_C)

clean :
			$(RM) $(OBJS_S) $(OBJS_C)

fclean :	clean
			$(RM) $(SERVER) $(CLIENT)

re :		fclean all

.PHONY : all clean fclean re
