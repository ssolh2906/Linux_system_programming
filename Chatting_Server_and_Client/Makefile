CC = gcc
CFLAGS =
LDFLAGS =  -lnsl -lpthread

.SUFFIXES : .c .o
.c.o :
	$(CC) -c $(CFLAGS) $<

ALL = chats chatc chats_select chatc_th

all: $(ALL)

chats: chats.o 
	$(CC) -o $@ $< $(LDFLAGS)
	
chats_select: chats_select.o
	$(CC) -o $@ $< $(LDFLAGS)

chatc: chatc.o 
	$(CC) -o $@ $< $(LDFLAGS)
	
chatc_th: chatc_th.o 
	$(CC) -o $@ $< $(LDFLAGS)

clean :
	rm -rf *.o $(ALL)
