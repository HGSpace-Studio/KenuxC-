CC = gcc
CFLAGS = -Wall -Wextra -O2 -std=c11 `pkg-config --cflags gtk+-2.0`
LDFLAGS = `pkg-config --libs gtk+-2.0`

SRC_DIR = .
OBJ_DIR = obj
BIN_DIR = bin

TARGET = $(BIN_DIR)/kenc-ide

SOURCES = ide.c
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
	install -m 755 $(TARGET) /usr/local/bin/kenc-ide

uninstall:
	rm -f /usr/local/bin/kenc-ide

run: $(TARGET)
	./$(TARGET)