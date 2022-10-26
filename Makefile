NAME = ircserv

CXX = clang++
CXXFLAGS = -Wall -Wextra -Werror -O3 -std=c++98 
DEBUGFLAGS = -O3 -g3 -fsanitize=address -fno-omit-frame-pointer
INC = -I .

SRCS = main.cpp Client.cpp Server.cpp Channel.cpp utils.cpp
OBJS = $(SRCS:.cpp=.o)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(INC) $(OBJS) -o $@

debug: $(OBJS)
	$(CXX) $(CXXFLAGS) $(DEBUGFLAGS) $(INC) $(OBJS) -o $(NAME) 

clean:
	rm -rf $(OBJS)
fclean: clean
	rm -rf $(NAME)
re: fclean all

.PHONY: all clean fclean re 
