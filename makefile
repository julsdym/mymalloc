CC = gcc
CFLAGS = -Wall -Wextra -g

TARGETS = test memgrind

SRC_COMMON = mymalloc.c
OBJ_COMMON = $(SRC_COMMON:.c=.o)

# Default target builds both
all: $(TARGETS)

# Build test
test: test.o $(OBJ_COMMON)
	$(CC) $(CFLAGS) -o $@ $^

# Build memgrind
memgrind: memgrind.o $(OBJ_COMMON)
	$(CC) $(CFLAGS) -o $@ $^

# Compile .c into .o (depends on header)
%.o: %.c mymalloc.h
	$(CC) $(CFLAGS) -c $< -o $@

# Run test program
run-test: test
	./test

# Run memgrind program
run-memgrind: memgrind
	./memgrind

# Remove build artifacts
clean:
	rm -f *.o $(TARGETS)
