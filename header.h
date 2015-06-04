/****************************
 ** Miles Curry - Chris Mendez
 ** CS-372-001 - Spring 2015
 ** Project
 ******************************/
#ifndef HEADER_H
 #define HEADER_H


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

#define PROMPT ">>>"

#define MAXLINE 4096

#define TRUE 1
#define FALSE 0
#define BUF_SIZE 256
#define LISTENQ 1024
#define MAXLINE 4096

/*** Commands ***/
#define CMD_ADD "add"
#define CMD_INBOX "inbox"
#define CMD_READ "read"
#define CMD_SEND "send"
#define CMD_LIST "list"
#define CMD_ISA "isa"


/* Gracefully taken from  a cs344 assignedment header created by Instructor Jessie R. Chaney */

/* Lengths and max sizes */
# define COMMAND_LENGTH 11
# define PAYLOAD_LENGTH 100
# define HEADER_FIELD 32
# define MAX_USER_NAME 32
# define MAX_SUBJECT 32
# define MAX_MESSAGE_LEN 10000
# define STATUS_CODE_LEN 4
# define IP_CHAR_LEN 16 /* Maximum IPv4 Dotted-decimal string */

typedef struct message_s{
    char status_code[STATUS_CODE_LEN];
    char ip_source[IP_CHAR_LEN] ;
    char ip_dest[IP_CHAR_LEN] ;
    char user_source[MAX_USER_NAME];
    char user_dest[MAX_USER_NAME];
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

/* Helper Functions */
void clearMsgStruct(message_t *msg);
int fillMessageHeader(message_t *msg, 
                     char status_code[STATUS_CODE_LEN], 
                     char ip_source[IP_CHAR_LEN], 
                     char ip_dest[IP_CHAR_LEN], 
                     char user_source[MAX_USER_NAME], 
                     char user_dest[MAX_USER_NAME], 
                     char message[MAX_MESSAGE_LEN]);

void printMessageHead(message_t *msg, int i);

void clearMsgStruct(message_t *msg){

}

/* Call this function to fill the message header */
int fillMessageHeader(message_t *msg, 
                     char status_code[STATUS_CODE_LEN], 
                     char ip_source[IP_CHAR_LEN], 
                     char ip_dest[IP_CHAR_LEN], 
                     char user_source[MAX_USER_NAME], 
                     char user_dest[MAX_USER_NAME], 
                     char message[MAX_MESSAGE_LEN]){
    
    sprintf(msg->status_code, "%s", status_code);
    sprintf(msg->ip_source, "%s", ip_source);
    sprintf(msg->ip_dest, "%s", ip_dest);
    snprintf(msg->user_source, MAX_USER_NAME, "%s", user_source);
    snprintf(msg->user_dest, MAX_USER_NAME, "%s", user_dest);
    snprintf(msg->message, MAX_MESSAGE_LEN, "%s", message);

    return 1;
}
void printMessageHead(message_t *msg, int i){
    printf("\t--Message--\n");
    printf("\tStatus Code = %s\n", msg->status_code);
    printf("\tIP_Source   = %s\n", msg->ip_source);
    printf("\tIP_Dest     = %s\n", msg->ip_dest);
    printf("\tUser Source = %s\n", msg->user_source);
    printf("\tUser To     = %s\n", msg->user_dest);
    printf("\tHeader Len  = %ld\n", msg->header_len);
    if(i > 0){
        printf("\tMessage = %s\n", msg->message);
    }
}

#endif 
