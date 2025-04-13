.PHONY: test clean

all: hashmap.o

tests: tests.c hashmap.c

test:
	make tests
	./tests

clean:
	rm *.o
