# Miles A. Curry
# currymi@onid.orst.edu
# CS344-001
# Assingment #5 - Makefile
#

CC=
CFLAGS= -Wall -Wshadow -Wunreachable-code -Wredundant-decls -Wmissing-declarations -Wold-style-definition -Wmissing-prototypes -Wdeclaration-after-statement
SERVER= socket_server.c
CLIENT= socket_client.c
MYHOST= mydbhost.c
MYHOST_HEADER= =/usr/local/classes/eecs/winter2015/cs344-001/src/Homework5/hostdb.h
SOCKET_HEADER= =/usr/local/classes/eecs/winter2015/cs344-001/src/Homework5/socket_hdr.h
TEST = test.c
SOURCES=


all	: $(SERVER) $(CLIENT) $(MYWORD)
	gcc -g -o socket_server -O $(SERVER) $(CFLAGS) -I $(SOCKET_HEADER)
	gcc -g -o socket_client -O $(CLIENT) $(CFLAGS) -I $(SOCKET_HEADER)
	gcc -g -o mydbhost -O $(MYHOST) $(CFLAGS) -I $(MYHOST_HEADER)

myhost : $(MYHOST)
	gcc -g -o mydbhost -O $(MYHOST) $(CFLAGS) -I $(MYHOST_HEADER)

socket : $(SERVER) $(CLIENT) $(MYWORD)
	gcc -g -o socket_server -O $(SERVER) $(CFLAGS) -I $(SOCKET_HEADER)
	gcc -g -o socket_client -O $(CLIENT) $(CFLAGS) -I $(SOCKET_HEADER)

server : $(SERVER)
	gcc -g -o socket_server -O $(SERVER) $(CFLAGS) -I $(SOCKET_HEADER)

client : $(CLIENT)
	gcc -g -o socket_client -O $(CLIENT) $(CFLAGS) -I $(SOCKET_HEADER)

server.o : $(SERVER)
	gcc -c $(CFLAGS) $(PROGS) 

client.o : $(CLIENT)
	gcc -c $(CFLAGS) $(PROGS) 

mydbhost.o : $(MYHOST)
	gcc -c $(CFLAGS) $(PROGS) 

test : $(TEST)
	gcc -g -o test $(CFLAGS) $(TEST)

clean:
	rm mydbhost
	rm socket_server
	rm client_server

