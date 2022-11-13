NAME = ircserv

CXX = clang++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 
DEBUGFLAGS = -g -fsanitize=address
INC = -I .

SRCS = main.cpp Client.cpp Server.cpp Channel.cpp Command.cpp RegistrationCommand.cpp Join.cpp Leave.cpp utils.cpp Invite.cpp Topic.cpp List.cpp Kick.cpp Message.cpp Whois.cpp Who.cpp Mode.cpp Nick.cpp Pass.cpp User.cpp Names.cpp

OBJS = $(SRCS:.cpp=.o)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(DEBUGFLSGS) $(INC) -c $< -o $@

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(DEBUGFLAGS) $(INC) $(OBJS) -o $@

debug: $(OBJS) 
	$(CXX) $(CXXFLAGS) $(DEBUGFLAGS) $(INC) $(OBJS) -o $(NAME) 

clean:
	rm -rf $(OBJS)
fclean: clean
	rm -rf $(NAME)
re: fclean all

.PHONY: all clean fclean re 
