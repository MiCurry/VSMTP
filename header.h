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

#define MAXLINE 4096

#define TRUE 1
#define FALSE 0
#define BUF_SIZE 256
#define LISTENQ 1024
#define MAXLINE 4096
