CC = gcc
CFLAGS =
LDFLAGS = -lpthread

.SUFFIXES : .c .o
.c.o :
	$(CC) -c $(CFLAGS) $<
	
ALL = shm sipc1 sipc2 sipc_sem sipc_m

all : $(ALL)

shm : shm.o
	$(CC) -o $@ $< $(LDFLAGS)
	
sipc1 : sipc1.o
	$(CC) -o $@ $< $(LDFLAGS)

sipc2 : sipc.o
	$(CC) -o $@ $< $(LDFLAGS)

sipc_sem : sipc_sem.o
	$(CC) -o $@ $< $(LDFLAGS)

sipc_m : sipc_m
	$(CC) -o $@ $< $(LDFLAGS)

clean :
	rm -rf *.o $(ALL)
