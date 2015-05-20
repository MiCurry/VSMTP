#########################################
## Miles A. Curry & Chris Mendez & Harrison
## CS-372-001 - Spring 2015
## Project
##########################################

## Makefile ##


CC=
CFLAGS= -Wall -Wshadow -Wunreachable-code -Wredundant-decls -Wmissing-declarations -Wmissing-prototypes -Wdeclaration-after-statement
SERVER= server.c
CLIENT= client.c
TEST = tests.c
SOURCES=


all	: $(SERVER) $(CLIENT) 
	gcc -g -o server -O $(SERVER) $(CFLAGS) 
	gcc -g -o client -O $(CLIENT) $(CFLAGS) 

server : $(SERVER) 
	gcc -g -o server -O $(SERVER) $(CFLAGS) 

client : $(CLIENT) 
	gcc -g -o client -O $(CLIENT) $(CFLAGS) 

test : $(TEST) 
	gcc -g -o test $(CFLAGS) $(TEST)

clean:
	rm server
	rm client
	rm test
