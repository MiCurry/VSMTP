/****************************
 ** Miles Curry - Chris Mendez
 ** CS-372-001 - Spring 2015
 ** Project
 ******************************/

/* Socket_client */
#define PROMPT ">>>"

#include "client.h"
#include "header.h"

flags_t flags;

int debug_value = 2;
static sig_atomic_t signal_recived = FALSE;

/** Helper functions **/
void usage(void){
    
}

/* Signal Handler's & On Exit Functions */
static void signal_handler(int signum){
    signal_recived = TRUE;
    exit(EXIT_SUCCESS);
}

/* Cleans up the mq at exit */
static void onexit_function(void){


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


    return 1;
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
        memset(&buffer, '\0', MAX_SIZE + 1);
        memset(&msg,  0, sizeof(msg));
        flags.no_pl_flag = 0;
        i = 0;
        j = 0;

        printf("%s", PROMPT);   
        fgets(buffer, MAX_SIZE, stdin);
        
        /* SPACE & BUFFER CHECK */
        while(buffer[i] != ' '){
            if(buffer[i] == '\n'){
                flags.no_pl_flag = 1;
                break;
            }
            i++;
        }

        snprintf(msg.command, i+1, "%s", buffer);
        printf("%s\n", command);

        if(flags.no_pl_flag != 1){
            i++;    //Move past the space
            for(j = 0 ; i < MAX_SIZE; i++){
                pl_buffer[j] = buffer[i];
                j++;

                if(buffer[i] == '\0'){
                    break;
                }
            }

            snprintf(msg.payload, j-1, "%s", pl_buffer);
        }

        msg.message_type = MESSAGE_TYPE_NORMAL;
        msg.num_bytes = sizeof(msg.command);

        if(debug_value > 1){
            printf("MT: %d \n", msg.message_type);
            printf("MC: %s \n", msg.command);
            printf("PL: %s \n", msg.payload);
            printf("NB: %d \n", msg.num_bytes);
        }

        if(strcmp(msg.command, "ADD") == 0){
            sendMsg(msg);
        }

    }
}

void sendMsg(message_t msg){
    message_t recv_msg;
    int numbytes;
    char recvline[MAXLINE];
    int sockfd;

    connectIP(PORT_NUM, FLIP1);

    memset(recvline, 0, sizeof(recvline));

    numbytes = write(sockfd, (char *) &msg, sizeof(message_t));
    if(numbytes == -1){
        perror("Write Error");
        exit(EXIT_FAILURE);
    }
    printf("Message Sent\n");

    fprintf(stdout, "from server: <%s>\n", recvline);
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

    /*** SERVER ***/
    shell();

    return 1;
}
