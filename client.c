/****************************
 ** Miles Curry - Chris Mendez
 ** CS-372-001 - Spring 2015
 ** Project
 ******************************/

/* Socket_client */

#include "client.h"
#include "header.h"

flags_t flags;

int debug_value = 2;
static sig_atomic_t signal_recived = FALSE;

/** Helper functions **/

void showHelp(void){
    
    printf("Chat Program");
    printf("Usage: -I IP_Address -P [port_number](Optional - Default = %d)\n", PORT_NUM);
    printf("\t -I: specify ip_address \n");
    printf("\t -P: specify port_number - (Optional - Default = %d) \n", PORT_NUM);
    printf("\t -h: Show this help message\n");

    exit(EXIT_SUCCESS);
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

/** Commands **/

int help(void){
    if(debug_value > 0){
        printf("DEBUG: client help called succesfully\n");
    }
    return 0;
}

/************************/


/***** CLIENT INIT ******/


/* Connect to IP ADDRESS */
int connectIP(int port, char ip[200]){
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

    /* SEND MESSAGE AND WAIT FOR MESSAGE BACK */

    while(fgets(sendline, sizeof(sendline), stdin) != NULL){
        sendline[strlen(sendline) - 1] = 0;
        memset(recvline, 0, sizeof(recvline));

        numbytes = write(sockfd, sendline, strlen(sendline));
        if(numbytes == -1){
            perror("Write Error");
            exit(EXIT_FAILURE);
        }

        if(read(sockfd,recvline, sizeof(recvline)) == 0){
            perror("Socket is closed");
            break;
        }

        fprintf(stdout, "from server: <%s>\n", recvline);
    }

    return 1;
}

void shell(void){

    sendMsg();
}


void sendMsg(void){


}



/* MAIN */
int main(int argc, char **argv, char **envp){
    int opt;
    char ip[100];
    int port = PORT_NUM;

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
                strcpy(ip, optarg);
                break;
            /* PORT NUMBER */
            case 'P': 
                if(debug_value > 0){ printf("DEBUG: PORT# = %s\n", optarg);
                }
                port = atoi(optarg);
                break;
            case 'h':
                showHelp();
                exit(EXIT_SUCCESS); // SHOUlDN"T GET HERE 
        }

    }


    /*** SERVER ***/
    connectIP(port, ip);
    shell();

    return 1;
}
