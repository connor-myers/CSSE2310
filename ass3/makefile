OPTS =	-std=gnu99 -pedantic -Wall -g

all: 2310dealer 2310A 2310B clean

2310dealer:	main.o errs.o utility.o init.o logic.o communication.o
	gcc $(OPTS) -o 2310dealer main.o errs.o utility.o init.o logic.o communication.o

2310A:	2310A.o players.o errs.o utility.o init.o logic.o communication.o
	gcc $(OPTS) -o 2310A 2310A.o players.o errs.o utility.o init.o logic.o communication.o

2310B:	2310B.o players.o errs.o utility.o init.o logic.o communication.o
	gcc $(OPTS) -o 2310B 2310B.o players.o errs.o utility.o init.o logic.o communication.o

main.o: 
	gcc $(OPTS) -c main.c

2310A.o: 
	gcc $(OPTS) -c 2310A.c

2310B.o: 
	gcc $(OPTS) -c 2310B.c

init.o:
	gcc $(OPTS) -c init.c

logic.o:
	gcc $(OPTS) -c logic.c

utility.o:
	gcc $(OPTS) -c utility.c	

errs.o:
	gcc $(OPTS) -c errs.c

players.o:
	gcc $(OPTS) -c players.c	

communication.o:
	gcc $(OPTS) -c communication.c		

clean:
	rm -f *.o *~ 