CC ?= cc
CFLAGS ?= -Wall -Wextra -Werror -std=c11 -I.
LDFLAGS ?=
LDLIBS ?= -lncursesw

BIN = setup_demo
SRC = \
	main.c \
	config.c \
	panel.c \
	screen_manager.c \
	vector.c \
	list_item.c \
	crt.c \
	function_bar.c

OBJ = $(SRC:.c=.o)

.DEFAULT_GOAL := all

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(LDFLAGS) -o $@ $(OBJ) $(LDLIBS)

clean:
	rm -f $(BIN) $(OBJ)

.PHONY: all clean
