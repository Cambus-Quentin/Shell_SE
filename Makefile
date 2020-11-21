all: shell

readline.o: readline.c
	gcc -c -g readline.c

shell: readline.o
	gcc -g -o shell readline.o

.PHONY: clear

clear:
	rm -f *.o shell