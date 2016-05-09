CC = gcc
CFLAGS = -Wall -std=gnu99

all: client server

client: DieWithMessage.o TCPEchoClient4.o
	$(CC) $(CFLAGS) -o client DieWithMessage.o TCPEchoClient4.o

server: DieWithMessage.o TCPEchoServer4.o TCPServerUtility.o AddressUtility.o
	$(CC) $(CFLAGS) -o server DieWithMessage.o TCPEchoServer4.o TCPServerUtility.o AddressUtility.o

DieWithMessage.o: DieWithMessage.c
	$(CC) $(CFLAGS) -c DieWithMessage.c

AddressUtility.o: AddressUtility.c
	$(CC) $(CFLAGS) -c AddressUtility.c

TCPEchoClient4.o: TCPEchoClient4.c Practical.h
	$(CC) $(CFLAGS) -c TCPEchoClient4.c

TCPEchoServer4.o: TCPEchoServer4.c Practical.h
	$(CC) $(CFLAGS) -c TCPEchoServer4.c

TCPServerUtility.o: TCPServerUtility.c Practical.h
	$(CC) $(CFLAGS) -c TCPServerUtility.c 

clean:
	rm -f client server *.o
