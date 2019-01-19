CC=gcc

RFLAGS= -Wall -O

DFLAGS= -Wall -g 

all: client

client: 
	$(CC) $(RFLAGS) main.c -o b.out -fopenmp

debug_client: 
	$(CC) $(DFLAGS) main.c -o b.out -fopenmp
	

clean: 
	rm -rf *.o client
