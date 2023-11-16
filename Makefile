NAME =		webserv

CXX =		c++
CXXFLAGS =	-std=c++98 -Wall -Wextra -Werror

OBJ_DIR =	build

SRC =		main Worker Server Route
OBJ =		$(addprefix $(OBJ_DIR)/, $(addsuffix .o, $(SRC)))

all: $(NAME)

$(NAME): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJ)

$(OBJ_DIR)/%.o: %.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

clean:
	@rm -rf $(OBJ_DIR)

fclean: clean
	@rm -f $(NAME)

re: fclean all