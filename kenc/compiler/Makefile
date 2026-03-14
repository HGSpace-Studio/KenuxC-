CC = gcc
CFLAGS = -Wall -Wextra -O2 -std=c11 -D_POSIX_C_SOURCE=200809L
LDFLAGS = 

SRC_DIR = .
OBJ_DIR = obj
BIN_DIR = bin

TARGET = $(BIN_DIR)/kenc

SOURCES = main.c lexer.c parser.c codegen.c
OBJECTS = $(addprefix $(OBJ_DIR)/, $(SOURCES:.c=.o))

.PHONY: all clean install uninstall

all: directories $(TARGET)

directories:
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(BIN_DIR)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

install: $(TARGET)
	install -d /usr/local/bin
	install -m 755 $(TARGET) /usr/local/bin/kenc
	install -d /usr/local/share/kenc
	install -m 644 *.h /usr/local/share/kenc/

uninstall:
	rm -f /usr/local/bin/kenc
	rm -rf /usr/local/share/kenc

test: $(TARGET)
	$(TARGET) examples/hello.kenp
	$(TARGET) examples/calc.kenp
	$(TARGET) examples/gui.kenp

debug: CFLAGS += -g -DDEBUG
debug: clean all

.PHONY: all clean install uninstall test debug