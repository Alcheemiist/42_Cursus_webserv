FLAGS = -Wall -Wextra -Werror -std=c++98

NAME = webserve

SRC =  ./networking/*.cpp  ./networking/request/*.cpp ./networking/response/*.cpp *.cpp\
	   ./config/*.cpp\

all:	$(NAME)

$(NAME) : $(SRC)
	c++  -fsanitize=address -g $(SRC) -o $(NAME) $(FLAGS)

clean:
	@rm -rf $(NAME)
	@rm -rf $(NAME).dSYM
	@rm -rf bodyfile
	@rm -rf log.txt
		
fclean:	clean

re:	fclean all

.PHONY: all clean fclean re

