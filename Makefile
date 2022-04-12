CC = gcc
LINKERFLAG = -lm

shell:		main.o shell.o  list.o signal.o
	${CC} main.o shell.o list.o signal.o -o shell

main.o:		main.c 	
	gcc -c main.c

shell.o: 	shell.c
	gcc -c shell.c

list.o: 	list.c
	gcc -c list.c

signal.o: 	signal.c
	gcc -c signal.c 

clean: 
	rm shell  
