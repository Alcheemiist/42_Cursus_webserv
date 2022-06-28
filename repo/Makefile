FLAGS = -Wall -Wextra -Werror -std=c++11

NAME = webserve

SRC =  ./ConfigFile/*.cpp ./networking/*.cpp  ./networking/request/*.cpp ./networking/response/*.cpp *.cpp\
		
OBJS = $(SRCS:%.c=%.o)

all:	$(NAME)

$(NAME) : $(SRC)
	@c++  $(FLAGS) $(SRC) -o $(NAME)

clean:
	@rm -rf $(NAME)
	@rm -rf bodyfile

fclean:	clean
	@rm -f $(NAME)

re:	fclean all

.PHONY: all clean fclean re

