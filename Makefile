CC = gcc
CFLAGS = -fsanitize=address -g 

SRCS = $(wildcard src/*.c)

PROGRAM=program

all: $(PROGRAM)

$(PROGRAM): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o $(PROGRAM)

clean:
	rm -f $(PROGRAM)

.PHONY: all clean
