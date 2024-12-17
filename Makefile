##
## EPITECH PROJECT, 2024
## SYNTHESE
## File description:
## SC
##

SRC	=	*.cpp

OBJ	=	$(SRC:.o)

NAME	=	serverJ2T3

all:	$(NAME)

$(NAME):	$(OBJ)
	g++	-std=c++20	-Wall	-Wextra	-Werror	-o	$(NAME)	$(SRC)
	rm	-f	*.o

clean:
	rm	-f	$(OBJ)

fclean:	clean
	rm	-f	$(NAME)

re:	fclean	all
