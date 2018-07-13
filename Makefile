all: index.c index.h
	gcc -Wall -Werror -o invertedIndex index.c

clean:
	rm invertedIndex
