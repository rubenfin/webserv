CC = c++ -std=c++11
CFLAGS = -g
SRC_DIR = src
SERVER_DIR = server
UTILS_DIR = utils
REQUEST_DIR = request
RESPONSE_DIR = response
LOG_DIR = extra/log

SRC :=  $(wildcard $(SRC_DIR)/*.cpp) \
		$(wildcard $(SRC_DIR)/$(SERVER_DIR)/*.cpp) \
		$(wildcard $(SRC_DIR)/$(UTILS_DIR)/*.cpp) \
		$(wildcard $(SRC_DIR)/$(REQUEST_DIR)/*.cpp) \
		$(wildcard $(SRC_DIR)/$(RESPONSE_DIR)/*.cpp) 
OBJS_DIR = objs
OBJ = $(SRC:$(SRC_DIR)/%.cpp=$(OBJS_DIR)/%.o)
NAME = webserv
INCL_DIRS := $(shell find . -type f -name "*.hpp" -exec dirname {} + | uniq)
INCLUDE_FLAG = $($(INCL_DIRS):%=-I%)
.PHONY: all clean fclean re
all: $(NAME)

clean:
	rm -rf $(OBJ)
	rm -rf $(LOG_DIR)
	@echo "$(YELLOW)Removed all objects!$(DEFAULT)"

fclean: clean
	rm -rf $(NAME)
	rm -rf $(OBJS_DIR)
	@echo "$(RED)Removed executables!$(DEFAULT)"
	@echo "$(RED)Removed libft static library in src directory!$(DEFAULT)"

$(LIBS_TARGET):
	$(MAKE) -C $(LIBFT_DIR)

library: $(LIBS_TARGET)

re: fclean all

$(NAME): $(OBJS_DIR) $(OBJ) $(LIBS_TARGET)
	$(CC) -o $(NAME) $(OBJ) $(LDFLAGS) $(LINKERFLAGS) $(LIBS_TARGET) $(LIBS_TARGET)
	@echo "$(GREEN)Compiled webserv!$(DEFAULT)"

$(OBJS_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(shell dirname $@)
	$(CC) $(CFLAGS) -c $< -o $@ $(INCLUDE_FLAG)

$(OBJS_DIR):
	mkdir -p $(OBJS_DIR)

#COLORS
RED = \033[1;31m
GREEN = \033[1;32m
YELLOW = \033[1;33m
DEFAULT = \033[0m