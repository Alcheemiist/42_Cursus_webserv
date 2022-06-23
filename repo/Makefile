FLAGS = -Wall -Wextra -Werror -std=c++98

NAME = webserve

SRC =  ./ConfigFile/*.cpp ./networking/*.cpp *.cpp\
		
OBJS = $(SRCS:%.c=%.o)

all:	$(NAME)

$(NAME) : $(SRC)
	@c++  $(FLAGS) $(SRC) -o $(NAME)

clean:
	@rm -rf $(NAME)

fclean:	clean
	@rm -f $(NAME)

re:	fclean all

.PHONY: all clean fclean re

