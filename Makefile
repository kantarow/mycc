CFLAGS=-std=c11 -g -static

mycc: mycc.c

test: mycc
		.test.sh

clean:
		rm -f myc *.o *~ tmp*

.PHONY: test clean
