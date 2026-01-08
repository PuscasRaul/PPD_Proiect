CC = gcc
CFLAGS = -W -Wall -Wextra -g -Isrc -I. 
LDFLAGS = -pthread 

# 1. Define the directory structure
SRC_DIR = src
OBJ_DIR = bin

# 2. List the filenames (without the src/ prefix for easier manipulation)
SOURCES = net.c server.c request.c logger.c \
          string_view.c hmap.c my_string.c thpool.c 

# 3. Use patsubst to turn 'net.c' into 'bin/net.o'
OBJECTS = $(patsubst %.c, $(OBJ_DIR)/%.o, $(SOURCES))

PROG = main

.PHONY: all clean

all: $(OBJ_DIR) $(PROG)

# Create the bin directory if it doesn't exist
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Link the program
# $^ contains bin/*.o and main.o
$(PROG): main.o $(OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# Compiles main.o (located in the root)
main.o: main.c
	$(CC) $(CFLAGS) -c $< -o $@

# Compiles src/%.c into bin/%.o
# $< is the source (src/filename.c)
# $@ is the target (bin/filename.o)
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) main.o $(PROG)
