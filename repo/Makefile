FLAGS = -Wall -Wextra -Werror -std=c++98 -fsanitize=address -g

NAME = webserve

SRC =  ./networking/*.cpp  ./networking/request/*.cpp ./networking/response/*.cpp *.cpp\
	   ./config/*.cpp\



OBJS = $(SRCS:%.c=%.o)

all:	$(NAME)

$(NAME) : $(SRC)
	@c++  -fsanitize=address -g $(SRC) -o $(NAME) 

clean:
	@rm -rf $(NAME)
	@rm -rf bodyfile
	@rm -rf log.txt
	
	
fclean:	clean

re:	fclean all

.PHONY: all clean fclean re

