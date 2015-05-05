/****************************
 ** Miles Curry - Chris Mendez
 ** CS-372-001 - Spring 2015
 ** Project
 ******************************/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <fcntl.h>
#include <signal.h>
#include <errno.h>

#include <pwd.h>

#include <linux/limits.h>
#include <fcntl.h>
#include <limits.h>

/* Threads */
#include <pthread.h>

/* Internet */
#include <dirent.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>

#define SERVER_CONFIRM "CONFIRMED"

/* RAD-ASS FLAG STRUCT */
typedef struct flags_s{ 
    unsigned int flag_v : 1;    // If 1 produces verbose output
    unsigned int debug_v = 0;
}flags_t;


// *** FUNCTION PROTOTYPES *** //
/* SIGNAL HANDLERS */
static void signal_handler(int signum);
static void onexit_function(void);


/* CLIENTS COMMANDS */
int exitCmd(void);
int lpwd(void);
int lcd(void);
int ldir(void);
int lhome(void);
int help(void);
int put(void);
int get(void);


/* CREATE SOCKET */
int connect(int port, char ip[200])
int connectIPV4(int port, char ip[200]);
int connectIPV6(int port, char ip[200]);
static void shell(void);
void sendMsg(void);
void showHelp(void);

