/****************************
 ** Miles Curry - Chris Mendez
 ** CS-372-001 - Spring 2015
 ** Project
 ******************************/

/* Socket_client */

#include "client.h"

#define TRUE 1
#define FALSE 0
#define BUF_SIZE 256

#define MAXLINE 4096

flags_t flags;

int debug_value = 2;
static sig_atomic_t signal_recived = FALSE;

/** Helper functions **/

void showHelp(void){


}

/* Assertions/Check */

/* Might use this function instead of writing if statements every time */
void assertInts(int value1, int value2, char[2] op){
    
    /* Could just do asserts in the function */

    /* ==
     * !=
     * >
     * <
     * =>
     * <=
     * ||
     * &&
     */



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




int connect(char ip[200], int port){

    /* Determine if IP address is ipv6 or ipv4 */
        /* ipv4 = 32 Bit - 172.16.254.1
         * ipv6 = 128 Bits - 2001:0DB8:AC10:FE01
         */

    


}




/* ConnectIPV4
 * This is a good function, but it only connects to IPV4. It would be nice to
 * be able to connect to IPV6 and IPV4 by checking the IP address that is
 * entered. Hence the above function */
int connectIPV4(int port, char ip[200]){
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
        if(debug_value > 0){
            printf("before fgets\n");
        }

    while(fgets(sendline, sizeof(sendline), stdin) != NULL){
        if(debug_value > 0){
            printf("after fgets\n");
        }
        sendline[strlen(sendline) - 1] = 0;
        memset(recvline, 0, sizeof(recvline));
    
        if(debug_value > 0){
            printf("Before write\n");
        }

        numbytes = write(sockfd, sendline, strlen(sendline));
        if(numbytes == -1){
            perror("Write Error");
            exit(EXIT_FAILURE);
        }

        if(debug_value > 0){
            printf("Before read\n");
        }

        if(read(sockfd,recvline, sizeof(recvline)) == 0){
            perror("Socket is closed");
            break;
        }
        if(debug_value > 0){
            printf("After read");
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
    int port = 0;

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
        signal(SIGHUP, signal_handler);     //SIGHUP because osu-wifi sucks 
    }
    /* Taken from Instructor Chaneys' show IP */ 
    if(argc < 2){
        fprintf(stderr, "Usage: client_server IP_address [port_number]\n");
        return(EXIT_FAILURE);
    }

    
    while((opt=getopt(argc, argv, "i:p:H:hv")) != -1){
        switch(opt){
            case 'v':
                flags.flag_v = 1;
                break;
            /* IP ADDRESS */
            case 'i':
                if(debug_value > 0){
                    printf("DEBUG: IP Addr = %s\n", optarg);
                }
                strcpy(ip, optarg);
                break;
            /* PORT NUMBER */
            case 'p': 
                if(debug_value > 0){
                    printf("DEBUG: PORT# = %s\n", optarg);
                }
                port = atoi(optarg);
                break;
            /* HOST NAME */
            case 'H':
            /*
                hn.ai_family = AF_UNSPEC;
                hn.ai_socktype = SOCK_STREAM;

                if((status = getaddrinfo(optarg, NULL, &hn, &res)) != 0){
                    perror("getaddrinfo error");
                    exit(EXIT_FAILURE);
                }
                //Assuming only one thing returned
                p = res;
                if(p->ai_family == AF_INET){
                    sai.sin_addr = (struct sockaddr_in *)p->ai_addr;
                }

                break;
             */
            case 'h':
                showHelp();
                exit(EXIT_SUCCESS); // SHOUlDN"T GET HERE 
        }

    }


    /*** SERVER ***/
    connectIPV4(port, ip);
    shell();

    return 1;
}
