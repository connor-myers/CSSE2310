OPTS =	-std=c99 -pedantic -Wall -g

push2310:	main.o load.o exit.o utility.o graphics.o computer.o input.o logic.o
	gcc $(OPTS) -o push2310 main.o load.o exit.o utility.o graphics.o computer.o input.o logic.o
	rm -f *.o *~ 

main.o: 
	gcc $(OPTS) -c main.c

load.o:
	gcc $(OPTS) -c load.c

exit.o:
	gcc $(OPTS) -c exit.c

utility.o:
	gcc $(OPTS) -c utility.c	

graphics.o:
	gcc $(OPTS) -c graphics.c

computer.o:
	gcc $(OPTS) -c computer.c

input.o:
	gcc $(OPTS) -c input.c

logic.o:
	gcc $(OPTS) -c logic.c



