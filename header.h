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
#include <mqueue.h>
#include <pthread.h>
#include <dirent.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>


#define PORT_NUM 10333

#define FLIP1 "128.193.54.226" // Address of flip1.engr.oregonstate.edu
#define FLIP2

#define MAXLINE 4096

#define TRUE 1
#define FALSE 0
#define BUF_SIZE 256
#define LISTENQ 1024
#define MAXLINE 4096

/* Gracefully taken from  a cs344 assignedment header created by Instructor Jessie R. Chaney */

# define COMMAND_LENGTH 11
# define PAYLOAD_LENGTH 100
# define HEADER_FIELD 32
# define MAX_USER_NAME 32
# define MAX_SUBJECT 32
# define MAX_MESSAGE_LEN 10000

typedef struct message_s{
    int status_code : 16;
    unsigned int ip_source : 32;
    unsigned int ip_dest : 32;
    char user_source[MAX_USER_NAME];
    char user_to[MAX_USER_NAME];
    char subject[MAX_SUBJECT];
    long header_len;
    char message[MAX_MESSAGE_LEN];
} message_t;

typedef struct message_s_1 {
  int message_type;
  char command[COMMAND_LENGTH];
  char payload[PAYLOAD_LENGTH];
  int num_bytes;
} message_t_1;

# define MAX_SIZE sizeof(message_t)


