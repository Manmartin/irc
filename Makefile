NAME = ircserv

CXX = clang++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 
DEBUGFLAGS = -g #-fsanitize=address
INC = -I .

SRCS = main.cpp Client.cpp Server.cpp Channel.cpp Command.cpp Join.cpp Leave.cpp utils.cpp
OBJS = $(SRCS:.cpp=.o)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(DEBUGFLAGS) $(INC) -c $< -o $@

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
