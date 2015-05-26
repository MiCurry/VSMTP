############################################################
# Name: Harrison Alan Kaiser
# Email: kaiserh@onid.oregonstate.edu
# Class: CS344-001 
# Assignment #4
# References: Class Materials
# Study Sessions
############################################################
#VARIABLES
CC = gcc
CFLAGS = -Wall -Wshadow -Wunreachable-code -Wredundant-decls \
	 -Wmissing-declarations -Wold-style-definition \
	 -Wmissing-prototypes -Wdeclaration-after-statement
SOURCES = mydbhost.c hostdb.h
OBJECTS = mydbhost.o
PROGS = socket_server socket_client mydbhost

#TARGETS
all: $(PROGS)

socket_server: socket_server.o
	$(CC) $(CFLAGS) -o $@ $^ -lrt

socket_server.o: socket_server.c socket_hdr.h
	$(CC) $(CFLAGS) -c $< -lrt

socket_client: socket_client.o
	$(CC) $(CFLAGS) -o $@ $^ -lrt

socket_client.o: socket_client.c socket_hdr.h
	$(CC) $(CFLAGS) -c $< -lrt

mydbhost: $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ -lrt

mydbhost.o: $(SOURCES)
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(PROGS) *.o *~
