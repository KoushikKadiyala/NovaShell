CC = gcc
CFLAGS = -Wall -Wextra -Werror -pedantic -g
INCLUDES = -Iinclude
SRC = CLI/main.c CLI/shell.c engine/parser.c engine/executor.c engine/builtins.c engine/environment.c engine/launcher.c engine/process.c
OBJ = $(SRC:.c=.o)
TARGET = NovaShell
LIBS = -lreadline

.PHONY: all clean distclean

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $^ $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(OBJ)

distclean: clean
	rm -f $(TARGET)
