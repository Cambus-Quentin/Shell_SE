all: shell

readline.o: readline.c 
	gcc -c -g readline.c 

intern_cmd.o: intern_cmd.c
	gcc -c -g intern_cmd.c

extern_cmd.o: extern_cmd.c
	gcc -c -g extern_cmd.c


shell.o: shell.c
	gcc -c -g shell.c

shell: readline.o intern_cmd.o shell.o extern_cmd.o
	gcc -g -o shell readline.o intern_cmd.o shell.o extern_cmd.o

.PHONY: clean

clean:
	rm -f *.o shell