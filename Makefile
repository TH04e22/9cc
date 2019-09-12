CFLAGS=-std=c11 -g -static

9cc: 9cc.c
	gcc -o 9cc 9cc.c

clean:
	rm -f 9cc *.o *~ tmp*

.PHONY: test clean
