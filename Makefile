NAME = webserv
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98

SRC = webserv.cpp Server.cpp Epoll.cpp 
OBJ = $(SRC:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(CXX) -o $(NAME) $(OBJ)

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all
