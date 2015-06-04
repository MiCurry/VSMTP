/****************************
 ** Miles Curry - Chris Mendez
 ** CS-372-001 - Spring 2015
 ** Project
 ******************************/

/* Socket_client */

#include "client.h"
#include "statusCodes.h"

flags_t flags;

int debug_value = 2;

static sig_atomic_t signal_recived = FALSE;

void signal_handler(int signum){ signal_recived = TRUE; exit(EXIT_SUCCESS);}
void onexit_function(void){ }


/************************/
/*****   CLIENT    ******/
int connectIP(int port, char ip[100]){
    struct sockaddr_in sa;
    int sockfd;
    int numbytes;
    char sendline[MAXLINE];
    char recvline[MAXLINE];

    /* CONVERT PRESENTATION IP TO NETWORK NAME*/
    inet_pton(AF_INET, ip, &sa.sin_addr.s_addr);
    sa.sin_port = htons(port);
    sa.sin_family = AF_INET;

    /* CREATE SOCKET */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(connect(sockfd, (struct sockaddr *) &sa, sizeof(sa)) != 0){
        perror("Could not connect");
        exit(EXIT_FAILURE);
    }

    if(debug_value > 0 ){
        printf("DEBUG: Connection Successfull \n");
    }

    return sockfd;
}



/****************************/
/****************************/
/********* SHELL ************/

/*** Shell Commands ***/
int help(void){
    if(debug_value > 0){
        printf("DEBUG: client help called succesfully\n");
    }
    return 0;
}
int client_add(char *params){
    if(debug_value > 0){
        printf("DEBUG: client_add\n");
    }
    return 1;
}

int client_inbox(char *params){
    if(debug_value > 0){
        printf("DEBUG: client_inbox\n");
    }
    return 1;
}

int client_read(char *params){
    if(debug_value > 0){
        printf("DEBUG: client_read\n");
    }
    return 1;
}

int client_send(char *params){
    if(debug_value > 0){
        printf("DEBUG: client_send\n");
    }
    return 1;
}

int client_list(char *params){
    if(debug_value > 0){
        printf("DEBUG: client_list\n");
    }
    return 1;
}

int client_isa(char *params){
    if(debug_value > 0){
        printf("DEBUG: client_isa\n");
    }
    return 1;
}


/*** ZE CLIENT ***/
void shell(void){
    int i, j;

    int argc;
    char *argv[MAX_SIZE];

    char buffer[MAX_SIZE];
    char command[MAX_SIZE];
    char pl_buffer[PATH_MAX];

    message_t msg;

    while(1){
        memset(&buffer, '\0', sizeof(buffer) + 1);
        memset(&msg,  0, sizeof(message_t));

        printf("%s", PROMPT);   
        fgets(buffer, sizeof(buffer), stdin);
        printf("DEBUG: %s\n", buffer);
        
        /* Message Decipher */
        if(0 == strncmp(buffer, CMD_ADD, strlen(CMD_ADD))){
            client_add(buffer);
            continue;
        }else if(0 == strncmp(buffer, CMD_INBOX, strlen(CMD_INBOX))){
            client_inbox(buffer);
            continue;
        }else if(0 == strncmp(buffer, CMD_READ, strlen(CMD_READ))){
            client_read(buffer);
            continue;
        }else if(0 == strncmp(buffer, CMD_SEND, strlen(CMD_SEND))){
            client_send(buffer);
            continue;
        }else if(0 == strncmp(buffer, CMD_LIST, strlen(CMD_LIST))){
            client_list(buffer);
            continue;
        }else if(0 == strncmp(buffer, CMD_ISA, strlen(CMD_ISA))){
            client_isa(buffer);
            continue;
        }


    }
}

void sendMsg(message_t msg){
    int numbytes;
    int sockfd = -1;
    message_t msg_r;

    sockfd = connectIP(PORT_NUM, FLIP1);

    numbytes = write(sockfd, (char *) &msg, sizeof(message_t));
    if(numbytes == -1){
        perror("Write Error");
        exit(EXIT_FAILURE);
    }
    if(debug_value > 0){
        printf("DEBUG: Message sent successfully!\n");
    }

    r_msg(msg_r, sockfd);

}

void r_msg(message_t msg_r, int sockfd){
    int nbytes;

    nbytes = read(sockfd, (char *) &msg_r, sizeof(message_t));

    if(debug_value > 0){
        printf("DEBUG: Message received from server!\n");
        printMessageHead(&msg_r, 1);
    }
}

void init(void){
    memset((void *) &flags, 0, sizeof(flags_t));
    umask(0);
    atexit(onexit_function);
    signal(SIGINT, signal_handler);
    signal(SIGHUP, signal_handler);     
    pthread_exit((void *) pthread_self());
}

int main(int argc, char **argv, char **envp){
    int opt;
    while((opt=getopt(argc, argv, "I:P:H:hv")) != -1){
        switch(opt){
            case 'v':
                flags.flag_v = 1;
                break;
            /* IP ADDRESS */
            case 'I':
                if(debug_value > 0){
                    printf("DEBUG: IP Addr = %s\n", optarg);
                }
                //strcpy(ip, optarg);
                break;
            /* PORT NUMBER */
            case 'P': 
                if(debug_value > 0){ printf("DEBUG: PORT# = %s\n", optarg);
                }
                //port = atoi(optarg);
                break;
            case 'h':
                break;
        }
    }

    pthread_t init_t; 
    pthread_create(&init_t, NULL, (void *) init, NULL);

    return 1;
}
