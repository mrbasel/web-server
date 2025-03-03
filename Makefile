CC = gcc
CFLAGS = -pthread -fsanitize=address -g -I src
SRCS = $(shell find src -name '*.c' ! -path "*/tests/*")
TEST_SRCS = src/tests/test.c
PROGRAM = program
TEST_PROGRAM = test_runner

all: $(PROGRAM)

$(PROGRAM): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o $(PROGRAM)

test: $(TEST_PROGRAM)
	./$(TEST_PROGRAM)

$(TEST_PROGRAM): $(TEST_SRCS) $(filter-out src/main.c, $(SRCS))
	$(CC) $(CFLAGS) $^ -o $(TEST_PROGRAM)

clean:
	rm -f $(PROGRAM) $(TEST_PROGRAM)

.PHONY: all clean test
