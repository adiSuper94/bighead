CC = clang
CFLAGS = -Wall -Wextra -Wpedantic -std=c99

all: testies

bighead.o: bighead.c bighead.h
	$(CC) $(CFLAGS) -c $< -o $@

testies: testies.c bighead.o
	$(CC) $(CFLAGS) -L./ -lbighead.o $< -o $@

.PHONY: clean
clean:
	rm -f testies *.o
