CC = gcc
FLAGS = -g -Wall
SRC = src
OUT = bin

OBJS = $(OUT)/logger.o $(OUT)/server.o $(OUT)/thpool.o

main: $(SRC)/main.c $(OBJS)
	$(CC) $(FLAGS) $(SRC)/main.c $(OBJS) -o $(OUT)/main

$(OUT)/server.o: $(SRC)/http/server.c
	$(CC) $(FLAGS) -c $(SRC)/http/server.c -o $(OUT)/server.o

$(OUT)/thpool.o: $(SRC)/utils/thpool.c
	$(CC) $(FLAGS) -c $(SRC)/utils/thpool.c -o $(OUT)/thpool.o

$(OUT)/logger.o: $(SRC)/utils/logger.c
	$(CC) $(FLAGS) -c $(SRC)/utils/logger.c -o $(OUT)/logger.o

clean:
	rm -rf $(OUT)/*
