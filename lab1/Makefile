CC=gcc

RFLAGS= -Wall -O

DFLAGS= -Wall -g 

all: client

client: 
	$(CC) $(RFLAGS) main.c -o a.out -lrt -lpthread 

debug_client: 
	$(CC) $(DFLAGS) main.c -o a.out -lrt -lpthread
	

clean: 
	rm -rf *.o client
