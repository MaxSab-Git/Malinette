TARGET = bin/Malinette
SRC_DIR = srcs/
OBJ_DIR = $(SRC_DIR)obj/
INCLUDE_DIR = include/
SOURCES = $(SRC_DIR)main.cpp \
			$(SRC_DIR)Test.cpp \
			$(SRC_DIR)GlobalTokenizer.cpp \
			$(SRC_DIR)Tokenizer.cpp \
			$(SRC_DIR)TestCommandTokenizer.cpp
OBJECTS = $(SOURCES:$(SRC_DIR)%.cpp=$(OBJ_DIR)%.o)
COMPILER = c++
FLAGS = -Wall -Wextra -Werror -O2

all:$(TARGET)

$(TARGET):$(OBJECTS)
	mkdir -p bin
	$(COMPILER) $(FLAGS) $(OBJECTS) -o $(TARGET)

$(OBJ_DIR)%.o:$(SRC_DIR)%.cpp
	mkdir -p $(OBJ_DIR)
	$(COMPILER) $(FLAGS) -I$(INCLUDE_DIR) -c $< -o $@

.PHONY: all clean fclean re launch

launch:$(TARGET)
	cd tests && ../$(TARGET) $(ARGS) > trace.txt

clean:
	rm -f $(OBJECTS)

fclean: clean
	rm -f $(TARGET)

re: fclean all
