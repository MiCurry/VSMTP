/****************************
 ** Miles Curry - Chris Mendez
 ** CS-372-001 - Spring 2015
 ** Project
 ******************************/

#ifndef CLIENT_H
 #define CLIENT_H

#include "header.h"

#define SERVER_CONFIRM "CONFIRMED"

/* RAD-ASS FLAG STRUCT */
typedef struct flags_s{ 
    unsigned int flag_v : 1;    // If 1 produces verbose output
    unsigned int no_pl_flag : 1;
}flags_t;

# define CREATE_USER_NAME "add"


#endif
