CC = c++
CFLAGS = -g -Wall -Wextra -Werror

SRC_DIR = src

SRC :=  $(wildcard $(SRC_DIR)/*.cpp)

OBJS_DIR = objs
OBJ = $(SRC:$(SRC_DIR)/%.c=$(OBJS_DIR)/%.o)

NAME = webserv

INCL_DIRS := $(shell find . -type f -name "*.hpp" -exec dirname {} + | uniq)
INCLUDE_FLAG = $($(INCL_DIRS):%=-I%)

.PHONY: all clean fclean re

all: $(NAME)

clean:
	@rm -rf $(OBJ)
	@echo "$(YELLOW)Removed all objects!$(DEFAULT)"
	$(MAKE) -C $(LIBFT_DIR) clean

fclean: clean
	@rm -rf $(NAME)
	@rm -rf $(OBJS_DIR)
	@echo "$(RED)Removed executables!$(DEFAULT)"
	$(MAKE) -C $(LIBFT_DIR) fclean
	@rm -f $(SRC_DIR)/libft.a
	@echo "$(RED)Removed libft static library in src directory!$(DEFAULT)"


$(LIBS_TARGET):
	@$(MAKE) -C $(LIBFT_DIR)

library: $(LIBS_TARGET)

re: fclean all

$(NAME): $(OBJS_DIR) $(OBJ) $(LIBS_TARGET)
	@$(CC) -o $(NAME) $(OBJ) $(LDFLAGS) $(LINKERFLAGS) $(LIBS_TARGET) $(LIBS_TARGET) -lreadline 
	@echo "$(GREEN)Compiled minishell!$(DEFAULT)"

$(OBJS_DIR)/%.o: $(SRC_DIR)/%.c
	echo $<
	@mkdir -p $(shell dirname $@)
	@$(CC) $(CFLAGS) -c $< -o $@ $(INCLUDE_FLAG)

$(OBJS_DIR):
	@mkdir -p $(OBJS_DIR)

#COLORS
RED = \033[1;31m
GREEN = \033[1;32m
YELLOW = \033[1;33m
DEFAULT = \033[0m