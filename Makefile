NAME	= philo

SRC		= philo.c utils.c parse.c routine.c

BONUS_SRC	= $(SRC:.c=_bonus.c)

SRC_DIR	= srcs/

BONUS_SRC_DIR	= srcs_bonus/

OBJ	= $(addprefix $(SRC_DIR), $(SRC:.c=.o))

BONUS_OBJ	= $(addprefix $(BONUS_SRC_DIR), $(BONUS_SRC:.c=.o))

INC_DIR = includes

CC		= cc

CFLAGS	= -Wall -Werror -Wextra -g -fsanitize=thread

%.o: %.c $(INC_DIR)/philo.h
	$(CC) $(CFLAGS) -I ./includes -c  $< -o $@

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME)

bonus: $(BONUS_OBJ)
	$(CC) $(CFLAGS) $(BONUS_OBJ) -o $(NAME)

clean:
	rm -f $(OBJ)
	rm -f $(BONUS_OBJ)

fclean: clean
	rm -f $(NAME)

re:	fclean all

.PHONY: all clean fclean re