CC = gcc
CFLAGS = -Wall -Wextra -Iinclude -g
LDFLAGS = 

SRC_DIR = src
INC_DIR = include
OBJ_DIR = build
TEST_DIR = tests

# Find all .c files in src subdirectories except main.c
SRCS = $(shell find $(SRC_DIR) -name "*.c" ! -name "main.c")
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))
MAIN_SRCS = $(SRC_DIR)/main.c
MAIN_OBJS = $(OBJ_DIR)/main.o

FINGERPRINT = fingerprint

all: $(FINGERPRINT) $(TEST_PARSER) $(TEST_ENCODING) $(TEST_INJECTOR)

$(FINGERPRINT): $(OBJS) $(MAIN_OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(TEST_PARSER): $(OBJS) $(TEST_DIR)/test_parser.c
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(TEST_ENCODING): $(OBJS) $(TEST_DIR)/test_encoding.c
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(TEST_INJECTOR): $(OBJS) $(TEST_DIR)/test_injector.c
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(FINGERPRINT) $(TEST_PARSER) $(TEST_ENCODING) $(TEST_INJECTOR) test_encoding_marked

.PHONY: all clean tests

tests: all
	./$(TEST_ENCODING)
	@echo "Test encoding completed."
	./$(TEST_INJECTOR)
	@echo "Test injector completed."
	@echo "To test parser, run: ./test_parser <elf_file>"
