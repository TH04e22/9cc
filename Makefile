CC = gcc
CFLAGS= -std=c11 -g -static

9cc: 9cc.c
	$(CC) -o 9cc 9cc.c

clean:
	-rm -f 9cc *.o *~ tmp*

.PHONY: test clean
