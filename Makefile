NAME	=	ircserv
SERVER	=	$(NAME)

SRCS_D	=	./
SRCS_F	=	$(addsuffix	.cpp,			\
				Server					\
				Server_cmd				\
				Channel					\
				Client					\
				main					\
			)
SRCS_S	=	$(addprefix $(SRCS_D), $(SRCS_F))

OBJS_D	=	./.obj/
OBJS_F	=	$(SRCS_F:.cpp=.o)
OBJS_S	=	$(addprefix $(OBJS_D), $(OBJS_F))

CXX		=	c++
CXXFLAGS	=	-Wall -Wextra -Werror -std=c++98
DBFLAGS	=	-g
RM		=	rm -rf

ifdef	DEBUG
		DBFLAGS		+=	-fsanitize=address -g3
		SERVER		=	$(addsuffix _debug, $(NAME))
endif

all	:	$(OBJS_D) $(SERVER)

$(OBJS_D)	:
	@if [ ! -d $(OBJS_D) ]; then \
		mkdir -p $(OBJS_D); \
	fi

$(SERVER)	:	$(OBJS_S)
			$(CXX) $(CXXFLAGS) $(DBFLAGS) -o $(SERVER) $(OBJS_S)

$(OBJS_D)%.o	:	$(SRCS_D)%.cpp
			$(CXX) $(CXXFLAGS) $(DBFLAGS) -c $< -o $@

debug	:
			@make DEBUG=1 all

clean	:
			$(RM) $(OBJS_D) $(OBJS_S)

fclean	:	clean
			$(RM) $(SERVER)

re	:	fclean all

.PHONY	:	all debug clean fclean re
