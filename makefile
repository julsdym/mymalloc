CC = gcc
CFLAGS = -Wall -Wextra -g

TARGETS = test memgrind

SRC_COMMON = mymalloc.c
OBJ_COMMON = $(SRC_COMMON:.c=.o)

all: $(TARGETS)

test: test.o $(OBJ_COMMON)
	$(CC) $(CFLAGS) -o $@ $^

memgrind: memgrind.o $(OBJ_COMMON)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c mymalloc.h
	$(CC) $(CFLAGS) -c $< -o $@

run-test: test
	./test

run-memgrind: memgrind
	./memgrind

clean:
	rm -f *.o $(TARGETS)
