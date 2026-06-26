# compiler
CC := gcc

# compiler flags
CFLAGS := -Wall -Wextra -Werror -pedantic -g -std=c11 -fsanitize=address

# include directories
INCLUDES := -Iinclude

# source files and target
SRC := CLI/main.c CLI/shell.c engine/core.c engine/parser.c engine/executor.c engine/builtins.c engine/environment.c engine/launcher.c engine/process.c engine/io.c engine/memory.c
OBJ := $(SRC:.c=.o)
TARGET := NovaShell

# libraries
LIBS := -lreadline

.PHONY: all clean fclean rebuild run

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $^ $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(TARGET)
run: $(TARGET)
	./$(TARGET)
rebuild: fclean all	