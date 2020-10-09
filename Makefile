all: shell

readline.o: readline.c
	gcc -c readline.c

shell: readline.o
	gcc -o shell readline.o

.PHONY: clear

clear:
	rm -f *.o shell