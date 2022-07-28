FLAGS = -Wall -Wextra -Werror -std=c++98
FS = -fsanitize=address -g 

NAME = webserve

SRC =  ./networking/*.cpp  ./networking/request/*.cpp ./networking/response/*.cpp *.cpp\
	   ./config/*.cpp\

all:	$(NAME)

$(NAME) : $(SRC)
	c++  $(SRC) -o $(NAME) $(FLAGS) $(FS)

clean:
	@rm -rf $(NAME)
	@rm -rf $(NAME).dSYM
	@rm -rf bodyfile
	@rm -rf log.txt
		
fclean:	clean

re:	fclean all

.PHONY: all clean fclean re

