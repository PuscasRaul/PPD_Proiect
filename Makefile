CC = gcc
CFLAGS = -W -Wall -Wextra -g -Isrc -I. 
LDFLAGS = -pthread 

SOURCES = src/net.c src/server.c src/request.c src/logger.c \
          src/string_view.c src/hmap.c src/my_string.c src/thpool.c 

OBJECTS = $(SOURCES:.c=.o)

PROG = main

.PHONY: all clean

all: $(PROG)

$(PROG): main.o $(OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) main.o $(PROG)
