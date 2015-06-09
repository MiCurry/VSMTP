/*                                              
**                                               client.c
**
**                                  VSMTP - VERY Simple Mail Transfer Protocl 
**
**                               CS 371 - Introduction To Networking - Spring 2015
**                                  Miles Curry - Chris Mendez - Harrison K
**                                        MohammadJavad NoroozOliaee
**                                         Oregon State University
**
** 
** 
** 
** 
** 
** 
** 
** 
** 
**
******************************/

#ifndef SEVER_H
#define SEVER_H

#include "header.h"

#define SERVER_CONFIRM "CONFIRMED"
#define PATH "~/Public/userNames"

/* RAD-ASS FLAG STRUCT */
typedef struct flags_s{ 
    unsigned int flag_v : 1;    // Produces verbose output
}flags_t;


#endif
