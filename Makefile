SERVER = ircserv

SRCS_D = ./
SRCS_F = $(addsuffix .cpp,			\
			Server					\
			Channel					\
			Client					\
			main					\
						)
SRCS_S = $(addprefix $(SRCS_D), $(SRCS_F))

OBJS_D = ./.obj/
OBJS_F = $(SRCS_F:.cpp=.o)
OBJS_S = $(addprefix $(OBJS_D), $(OBJS_F))

CXX = c++
FLAGS = -Wall -Wextra -Werror -std=c++98 -g
RM = rm -rf

all : 		$(OBJS_D) $(SERVER)

$(OBJS_D)	:
	@if [ ! -d $(OBJS_D) ]; then \
		mkdir -p $(OBJS_D); \
	fi

$(SERVER) :	$(OBJS_S)
			$(CXX) $(FLAGS) -o $(SERVER) $(OBJS_S)

$(OBJS_D)%.o : $(SRCS_D)%.cpp
			$(CXX) $(FLAGS) -c $< -o $@

clean :
			$(RM) $(OBJS_D) $(OBJS_S)

fclean :	clean
			$(RM) $(SERVER)

re :		fclean all

.PHONY : all clean fclean re
