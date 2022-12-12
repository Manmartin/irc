NAME = ircserv

CXX = clang++
CXXFLAGS = -Wall -Wextra -Werror -O2 -std=c++98 -pedantic
DEBUGFLAGS = -Wall -Wextra -Werror -std=c++98 -pedantic -g3 -fsanitize=address
INC = -I ./ -I server/ -I commands/

SRCS = main.cpp Client.cpp Channel.cpp Join.cpp Leave.cpp utils.cpp Invite.cpp Topic.cpp List.cpp Kick.cpp Message.cpp Whois.cpp Who.cpp Mode.cpp Nick.cpp Pass.cpp User.cpp Names.cpp

COMMANDS_F	= 	Command.cpp

SERVER_F 	=	Server.cpp


SRCS += $(addprefix commands/, $(COMMANDS_F))
SRCS += $(addprefix server/, $(SERVER_F))

OBJS = $(SRCS:.cpp=.o)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(INC) $(OBJS) -o $@

debug: CXXFLAGS = $(DEBUGFLAGS)
debug: $(OBJS) 
	$(CXX) $(CXXFLAGS) $(INC) $(OBJS) -o $(NAME) 

clean:
	rm -rf $(OBJS)
fclean: clean
	rm -rf $(NAME)
re: fclean all

.PHONY: all clean fclean re 
