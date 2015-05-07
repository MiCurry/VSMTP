/****************************
 ** Miles Curry - Chris Mendez
 ** CS-372-001 - Spring 2015
 ** Project
 ******************************/

#include "header.h"

#define SERVER_CONFIRM "CONFIRMED"

/* RAD-ASS FLAG STRUCT */
typedef struct flags_s{ 
    unsigned int flag_v : 1;    // If 1 produces verbose output
    unsigned int no_pl_flag : 1;
}flags_t;


# define CREATE_USER_NAME "add"

// *** FUNCTION PROTOTYPES *** //
/* SIGNAL HANDLERS */
static void signal_handler(int signum);
static void onexit_function(void);


/* CLIENTS COMMANDS */
int getMessages(void);
int getUsers(void);
int sendMessage(void);
int readInbox(void);
int saveMessage(void);
int saveInbox(void);

int deleteMessage(void);
    
int createUser(void);
int lookUpUser(void);
int deleteUser(void);


/* CREATE SOCKET */
int connectIP(int port, char ip[200]);
void shell(void);
void sendMsg(message_t msg);
void showHelp(void);
void usage(void);

