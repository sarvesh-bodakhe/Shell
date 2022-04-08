CC = gcc
LINKERFLAG = -lm

shell:	main.c shell.c  list.c signal.c
	${CC} main.c shell.c  list.c signal.c -o shell

clean: 
	rm shell
