UNAME_S =		$(shell uname -s)
ifeq ($(UNAME_S),Darwin)
	NAME =		liblogging.dylib
else
	NAME =		liblogging.so
endif

CXX =		c++
CXXFLAGS =	-std=c++98 -Wall -Wextra -Werror

OBJ_DIR =	build

SRC =		logging
OBJ =		$(addprefix $(OBJ_DIR)/, $(addsuffix .o, $(SRC)))

LIB_PATH =	$(HOME)/.local/lib

all: $(NAME)

$(NAME): $(OBJ)
	$(CXX) $(CXXFLAGS) -Qunused-arguments -shared \
		-install_name @rpath/$(NAME) $(OBJ) -o $(NAME)
	@mkdir -p $(LIB_PATH)
	@cp $(NAME) $(LIB_PATH)/

$(OBJ_DIR)/%.o: %.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -fPIC -c $< -o $@

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

clean:
	@rm -rf $(OBJ_DIR)

fclean: clean
	@rm -f $(LIB_PATH)/$(NAME)
	@rm -f $(NAME)

re: fclean all
