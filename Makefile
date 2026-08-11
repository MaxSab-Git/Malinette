TARGET = bin/Malinette
SRC_DIR = srcs/
SRC_TOKENIZER_DIR = $(SRC_DIR)Tokenizer/
SRC_PARSER_DIR = $(SRC_DIR)Parser/
OBJ_DIR = $(SRC_DIR)obj/
INCLUDE_DIR = include/
SOURCES = $(SRC_DIR)main.cpp \
			$(SRC_DIR)Test.cpp \
			$(SRC_DIR)SystemProcess.cpp \
			$(SRC_TOKENIZER_DIR)GlobalTokenizer.cpp \
			$(SRC_TOKENIZER_DIR)TestCommandTokenizer.cpp \
			$(SRC_TOKENIZER_DIR)Tokenizer.cpp \
			$(SRC_PARSER_DIR)TestNameParser.cpp \
			$(SRC_PARSER_DIR)TestContextParser.cpp \
			$(SRC_PARSER_DIR)InstructionParser.cpp \
			$(SRC_PARSER_DIR)CommandParser.cpp \
			$(SRC_PARSER_DIR)FunctionParser.cpp \
			$(SRC_PARSER_DIR)LaunchFunctionParser.cpp \
			$(SRC_PARSER_DIR)GetterParser.cpp \
			$(SRC_PARSER_DIR)ParameterParser.cpp \
			$(SRC_PARSER_DIR)ParserState.cpp \
			$(SRC_PARSER_DIR)Parser.cpp
OBJECTS = $(SOURCES:$(SRC_DIR)%.cpp=$(OBJ_DIR)%.o)
COMPILER = c++
FLAGS = -Wall -Wextra -Werror -O2 -std=c++17
ARGS = tests/subjects.mali -DTypes -DValues

all:$(TARGET)

$(TARGET):$(OBJECTS)
	mkdir -p bin
	$(COMPILER) $(FLAGS) $(OBJECTS) -o $(TARGET)

$(OBJ_DIR)%.o:$(SRC_DIR)%.cpp
	mkdir -p $$(dirname $@)
	$(COMPILER) $(FLAGS) -I$(INCLUDE_DIR) -c $< -o $@

.PHONY: all clean fclean re launch lightcmd

launch:$(TARGET)
	$(TARGET) $(ARGS) > trace.txt

lightcmd: $(TARGET)
	$(TARGET) "tests/testfiles/lightcmd/compile_light_cmd.mali"

clean:
	rm -f $(OBJECTS)

fclean: clean
	rm -f $(TARGET)

re: fclean all
