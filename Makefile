NAME =			webserv

CXX =			c++

CXXFLAGS =		-std=c++98 -Wall -Wextra -Werror

OBJ_DIR =		build
SRC_DIR =		srcs
INC_DIR =		includes

SRC =			main Worker Server ServerGroup Route Request Response \
				worker_loop_darwin worker_loop_linux better_string \
				ParseUtils cgi StatusCodes MimeTypes 
OBJ =			$(addprefix $(OBJ_DIR)/, $(addsuffix .o, $(SRC)))

LIBLOGGING =	liblogging
LIB_PATH =		$(HOME)/.local/lib/
LIBS =			-L./$(LIBLOGGING) -llogging 

UNAME_S =		$(shell uname -s)
ifeq ($(UNAME_S),Darwin)
	LIBS +=		-rpath $(LIB_PATH)
else
	LIBS +=		-Wl,-R$(LIB_PATH)
endif

all: $(NAME)

$(NAME): $(OBJ) | logging
	$(CXX) $(CXXFLAGS) -I./$(INC_DIR) $(OBJ) $(LIBS) -o $(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -I./$(INC_DIR) -c $< -o $@

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

logging:
	@make -C $(LIBLOGGING)

clean:
	@rm -rf $(OBJ_DIR)

fclean: clean
	@rm -f $(NAME)
	@make -C $(LIBLOGGING) fclean

re: fclean all
