FLAGS = -Wall -Wextra -Werror -std=c++11

NAME = webserve

<<<<<<< HEAD
SRC =  ./ConfigFile/*.cpp ./ConfigFile/*.hpp ./networking/*.cpp *.cpp\
=======
SRC =  ./ConfigFile/*.cpp ./networking/*.cpp  ./networking/request/*.cpp ./networking/response/*.cpp *.cpp\
>>>>>>> 1ac5696e3f5504f3c3e8bfd1968a617f58c9d45f
		
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

