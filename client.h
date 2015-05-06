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

/* Gracefully taken from  a cs344 assignedment header created by Instructor Jessie R. Chaney */
# define COMMAND_LENGTH  11
# define MAX_SIZE sizeof(message_t)
# define PAYLOAD_LENGTH  PATH_MAX

typedef struct message_s {
  int message_type;
  char command[COMMAND_LENGTH];
  char payload[PAYLOAD_LENGTH];
  int num_bytes;
} message_t;

# define MESSAGE_TYPE_ERROR    -1
# define MESSAGE_TYPE_NORMAL   0
# define MESSAGE_TYPE_DIR      1
# define MESSAGE_TYPE_DIR_END  2
# define MESSAGE_TYPE_SEND     3
# define MESSAGE_TYPE_SEND_END 4

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
int connectIP(int port, char ip[200]);
static void shell(void);
void sendMsg(int sockfd);
void showHelp(void);

