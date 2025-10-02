CC = gcc
CFLAGS = -Wall -Wextra -g

TARGETS = test memgrind

all: $(TARGETS)

test: test.o mymalloc.o
	$(CC) $(CFLAGS) -o test test.o mymalloc.o

memgrind: memgrind.o mymalloc.o
	$(CC) $(CFLAGS) -o memgrind memgrind.o mymalloc.o

%.o: %.c mymalloc.h
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o $(TARGETS)
