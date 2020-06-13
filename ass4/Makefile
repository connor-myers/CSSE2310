OPTS =	-Wall -pedantic -g -pthread -std=gnu99

all: mapper2310 control2310 roc2310 clean

mapper2310:	mapper2310.o network.o utility.o arrayList.o
	gcc $(OPTS) -o mapper2310 mapper2310.o network.o utility.o arrayList.o

control2310:	control2310.o exit.o network.o utility.o arrayList.o
	gcc $(OPTS) -o control2310 control2310.o exit.o network.o utility.o arrayList.o

roc2310:	roc2310.o exit.o utility.o network.o arrayList.o
	gcc $(OPTS) -o roc2310 roc2310.o exit.o utility.o network.o arrayList.o

mapper2310.o: 
	gcc $(OPTS) -c mapper2310.c

control2310.o:
	gcc $(OPTS) -c control2310.c	

roc2310.o:
	gcc $(OPTS) -c roc2310.c

exit.o:
	gcc $(OPTS) -c exit.c

network.o:
	gcc $(OPTS) -c network.c

utility.o:
	gcc $(OPTS) -c utility.c

arrayList.o:
	gcc $(OPTS) -c arrayList.c

clean:
	rm -f *.o *~ 	