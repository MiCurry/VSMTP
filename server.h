/* Miles A. Curry
 * currymi@onid.orst.edu
 * CS344-001
 * Assignment #4 
 */

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
#include <mqueue.h>
#include <pthread.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/select.h>


#define SERVER_CONFIRM "CONFIRMED"

/* RAD-ASS FLAG STRUCT */
typedef struct flags_s{ 
    unsigned int flag_v : 1;    // Produces verbose output
}flags_t;

/* SIGNAL HANDLERS AND EXIT FUNCTIONS */
static void onexit_function(void);
static void signal_handler(int signum);


/* SERVER COMMANDS */
int cdCmd(void);
int dirCmd(void);
int pwdCmd(void);
int homeCmd(void);
void put(void);
void getCmd(void);
void showHelp(void);

/* SERVER IP CONNECTION */
void recMsgs(void);
int createPort(void);
