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

/** Helper functions **/
void usage(void){
    
}

/* Signal Handler's & On Exit Functions */
void signal_handler(int signum){
    signal_recived = TRUE;
    exit(EXIT_SUCCESS);
}

/* Cleans up the mq at exit */
void onexit_function(void){


}



/************************/
/***** CLIENT INIT ******/
/* Connect to IP ADDRESS */
int connectIP(int port, char ip[100]){
    /* PORT = port number
     * ip = ip in human readable format
     * family = either 4 or 6 for IPv4 or IPv6
     */
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
/*************************/

void shell(void){
    int i, j;

    char buffer[MAX_SIZE];
    char command[MAX_SIZE];
    char pl_buffer[PATH_MAX];

    message_t msg;

    while(1){
        memset(&buffer, '\0', sizeof(buffer) + 1);
        memset(&msg,  0, sizeof(message_t));

        printf("%s", PROMPT);   
        fgets(buffer, sizeof(buffer), stdin);
        
        /* SPACE & BUFFER CHECK */


    }
}

void sendMsg(message_t msg){
    message_t recv_msg;
    int numbytes;
    int sockfd = -1;

    sockfd = connectIP(PORT_NUM, FLIP1);

    numbytes = write(sockfd, (char *) &msg, sizeof(message_t));
    if(numbytes == -1){
        perror("Write Error");
        exit(EXIT_FAILURE);
    }
    if(debug_value > 0){
        printf("DEBUG: Message sent successfully!");
    }
}


/* MAIN */
int main(int argc, char **argv, char **envp){
    int opt;

    memset((void *) &flags, 0, sizeof(flags_t));

    /* INIT FUNCTIONS */
    {
        umask(0);

        /* DEBUG MESG TEMPLATE */
        if(debug_value > 0 ){
            printf("DEBUG: \n");
        }

        /* AT EXIT AND SIGNAL HANDLERS */
        atexit(onexit_function);
        signal(SIGINT, signal_handler);
        signal(SIGHUP, signal_handler);     
    }

    /* Taken from Instructor Chaneys' show IP */ 
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
                printf("Chat Program");
                printf("Usage: -I IP_Address -P [port_number](Optional - Default = %d)\n", PORT_NUM);
                printf("\t -I: specify ip_address \n");
                printf("\t -P: specify port_number - (Optional - Default = %d) \n", PORT_NUM);
                printf("\t -h: Show this help message\n");

                exit(EXIT_SUCCESS);
        }

    }


    if(debug_value > 0){
        //printf("IP = %s\n", ip);
        //printf("PORT = %d\n", port);
    }

    message_t msg_1;
    message_t msg_2;

    fillMessageHeader(&msg_1, "300", "1.1.1.1", "2.2.2.2", "Miles", "Jessica", "This is my message!");
    printMessageHead(&msg_1, 1);

    fillMessageHeader(&msg_2, MSG_TYPE_CMD, "0.0.0.0", FLIP1, "Miles", "Jessica", "A Message to flip!");
    printMessageHead(&msg_2, 1);
    sendMsg(msg_2);

    return 1;
}
