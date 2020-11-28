all: shell

readline.o: readline.c 
	gcc -c -g readline.c 

intern_cmd.o: intern_cmd.c
	gcc -c -g intern_cmd.c

shell: readline.o intern_cmd.o
	gcc -g -o shell readline.o intern_cmd.o

.PHONY: clean

clean:
	rm -f *.o shell