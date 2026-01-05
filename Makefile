CC = gcc
CFLAGS = -g -Wall -Iinclude -Isrc  # Added -I so you don't need ../ in includes
OUT = bin
SRC_DIR = src
UTILS_DIR = src/utils
HTTP_DIR = src/http
TEST_DIR = src/tests

# 1. Automatically find all source files in subdirectories
SRCS = $(wildcard $(UTILS_DIR)/*.c) $(wildcard $(HTTP_DIR)/*.c)
# 2. Convert source paths to object paths in the bin directory
OBJS = $(patsubst %.c, $(OUT)/%.o, $(notdir $(SRCS)))

# Targets
.PHONY: all clean run_tests

all: $(OUT)/main

# Pattern Rule: How to turn any .c file into a .o file in the bin directory
# % is a wildcard, $< is the source file, $@ is the target
$(OUT)/%.o: $(UTILS_DIR)/%.c | $(OUT)
	$(CC) $(CFLAGS) -c $< -o $@

$(OUT)/%.o: $(HTTP_DIR)/%.c | $(OUT)
	$(CC) $(CFLAGS) -c $< -o $@

# Build Main
$(OUT)/main: $(SRC_DIR)/main.c $(OBJS) | $(OUT)
	$(CC) $(CFLAGS) $^ -o $@

# Build and Run Tests
run_tests: $(OUT)/string_tests $(OUT)/req_tests
	./$(OUT)/string_tests
	./$(OUT)/req_tests

$(OUT)/string_tests: $(TEST_DIR)/my_string_test.c $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

$(OUT)/req_tests: $(TEST_DIR)/http_request_test.c $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

$(OUT):
	mkdir -p $(OUT)

clean:
	rm -rf $(OUT)
