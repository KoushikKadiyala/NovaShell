CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=gnu11 -g
INCLUDES = -Iinclude
SRC = src/main.c src/shell.c src/parser.c src/executor.c src/builtins.c src/environment.c src/launcher.c src/process.c
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
