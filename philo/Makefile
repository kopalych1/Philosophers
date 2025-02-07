# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: akostian <akostian@student.s19.be>         +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/08/19 13:20:29 by akostian          #+#    #+#              #
#    Updated: 2025/02/07 05:27:52 by akostian         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #


NAME				= philo

VPATH				= src

SRC_DIR				= src
SRCS				= main.c \
					  parse.c is_all_digit.c ft_atoi.c ft_imax.c \
					  ft_calloc.c init.c \
					  philo.c \
					  free.c \
					  get_time.c

INC_DIR				= include
INCLUDES			= -I$(INC_DIR)

BUILD_DIR			= build
OBJS				= $(addprefix $(BUILD_DIR)/, $(SRCS:%.c=%.o))

CC					= cc
CFLAGS				= -Wall -Wextra -Werror -g3

NORMINETTE			= norminette
NORMINETTE_FLAGS	= --use-gitignore

RM					= rm -rf



all: $(NAME)
#


$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) $(OBJS) -o $(NAME)
#


$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)
#


$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@
#


run: all
	./$(NAME)
#


vrun: all
	valgrind ./$(NAME)
#


norm:
	$(NORMINETTE) $(NORMINETTE_FLAGS) $(SRC_DIR) $(INC_DIR)
#


clean:
	$(RM) $(BUILD_DIR)
#


fclean:
	$(RM) $(BUILD_DIR) $(NAME)
#


re: fclean all
#



.PHONY: all clean fclean re