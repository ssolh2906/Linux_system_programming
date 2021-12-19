CC = gcc
CFLAGS =
LDFLAGS =  -lnsl -lpthread

.SUFFIXES : .c .o
.c.o :
	$(CC) -c $(CFLAGS) $<

ALL = chats chatc chats1

all: $(ALL)

chats: chats.o 
	$(CC) -o $@ $< $(LDFLAGS)
	
chats1: chats1.o
	$(CC) -o $@ $< $(LDFLAGS)

chatc: chatc.o 
	$(CC) -o $@ $< $(LDFLAGS)

clean :
	rm -rf *.o $(ALL)
