/* Miles A. Curry
 * currymi@onid.orst.edu
 * CS344-001
 * Assignment #4 
 */
#ifndef SEVER_H
#define SEVER_H

#include "header.h"

#define SERVER_CONFIRM "CONFIRMED"
#define PATH "~/Public/userNames"

/* RAD-ASS FLAG STRUCT */
typedef struct flags_s{ 
    unsigned int flag_v : 1;    // Produces verbose output
}flags_t;

/* SIGNAL HANDLERS AND EXIT FUNCTIONS */
static void onexit_function(void);
static void signal_handler(int signum);


/* SERVER COMMANDS */
int createFile(char *name);
int cdCmd(void);
int dirCmd(void);
int pwdCmd(void);
int homeCmd(void);
void put(void);
void getCmd(void);
void showHelp(void);

/* SERVER IP CONNECTION */
int sendMsg(message_t msg, int sockfd);
int reciveMsg(message_t msg, int sockFD);
int serverInit(void);
int createIPV4(int port);

#endif
