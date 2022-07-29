FLAGS = -Wall -Wextra -Werror -std=c++98 -g
FS = -fsanitize=address -g 

NAME = webserve

# SRC =  ./networking/*.cpp  ./networking/request/*.cpp ./networking/response/*.cpp *.cpp ./config/*.cpp
SRC =	./config/AllowedComponent.cpp \
		./config/Component.cpp \
		./config/LexicalError.cpp \
		./config/LogicalError.cpp \
		./config/ParsingError.cpp \
		./config/cgi.cpp \
		./config/location.cpp \
		./config/parse_confile.cpp \
		./config/parse_main.cpp \
		./config/server.cpp \
		./config/utils.cpp \
		./networking/elements.cpp \
		./networking/request/Request.cpp \
		./networking/response/Response.cpp \
		./networking/response/Response_utiles.cpp \
		server.cpp
all:	$(NAME)

$(NAME) : $(SRC)
	c++  $(SRC) -o $(NAME) $(FLAGS) 

clean:
	@rm -rf $(NAME)
	@rm -rf $(NAME).dSYM
	@rm -rf bodyfile
	@rm -rf log.txt
		
fclean:	clean

re:	fclean all

.PHONY: all clean fclean re

