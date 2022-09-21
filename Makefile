NAME = ircserv

CXX = clang++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98
INC = -I .

SRCS = main.cpp Client.cpp Server.cpp Channel.cpp Reply.cpp
OBJS = $(SRCS:.cpp=.o)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(INC) $(OBJS) -o $@


clean:
	rm -rf $(OBJS)
fclean: clean
	rm -rf $(NAME)
re: fclean all

.PHONY: all clean fclean re 
