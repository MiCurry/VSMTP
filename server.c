/****************************
 ** Miles Curry - Chris Mendez
 ** CS-372-001 - Spring 2015
 ** Project
 ******************************/

#include "server.h"
#include "header.h"

#define TRUE 1
#define FALSE 0
#define BUF_SIZE 256
#define LISTENQ 1024
#define MAXLINE 4096

#define MAX_CLIENTS FD_SETSIZE
#define NOT_IN_USE -1

int debug_value = 4;
flags_t flags;
static sig_atomic_t signal_recived = FALSE;


/* Signal Handler's & On Exit Functions */

static void signal_handler(int signum){
    signal_recived = TRUE;
    exit(EXIT_SUCCESS);
}

/* Cleans up the mq at exit */
static void onexit_function(void){
    

}

void showHelp(void){
    printf("Chat Program Server");
    printf("Usage: -P [port_number](Optional - Default = %d)\n", PORT_NUM);
    printf("\t -P: specify port_number - (Optional - Default = %d) \n", PORT_NUM);
    printf("\t -h: Show this help message\n");

    exit(EXIT_SUCCESS);
}

/**** Helper Functions *****/

/*vvvvvvvvvvvvv COMMANDS START  vvvvvvvvvvvvvvvv*/

/*^^^^^^^^^^^    COMMANDS END    ^^^^^^^^^^^^^^*/
/* SERVER SPECIFIC FUNCTIONS */
/* CREATES OUR IPV4 STREAM SOCKET */
int createIPV4(int port){
    int listenfd, sockfd, connfd;
    int client[MAX_CLIENTS];
    int maxfd;
    int nready;
    int nclients;
    int nbytes;
    int i;
    fd_set allset;
    fd_set rset;
    socklen_t clilen;

    char buf[MAXLINE];

    struct sockaddr_in servaddr;   
    struct sockaddr_in cliaddr;

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htons(INADDR_ANY);
    servaddr.sin_port = htons(port);   

    listenfd = -1;
    sockfd = -1;
    connfd = -1;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if(bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) != 0){
        perror("Could not bind");
        exit(EXIT_FAILURE);
    }
    if(debug_value > 3 ){
        printf("DEBUG: Socket and Bind successful\n");
    }

    /* THIS IS THE FD WE ARE GOING TO BE LISTING FOR NEW CONNECTIONS */
    listen(listenfd, LISTENQ);
    if(debug_value > 3 ){
        printf("DEBUG: Server listing on port %d\n", port);
    }

    maxfd = listenfd;  

    /* SET OUR ARRAY OF CLIENTS TO UNUSED */
    for(i = 0; i <= MAX_CLIENTS; i++){
        client[i] = NOT_IN_USE;
    }

    /* ZERO OUT FDSETS */
    FD_ZERO(&allset);
    FD_ZERO(&rset);

    /* ADD THE LISTENFD TO THE ALLSET */
    FD_SET(listenfd, &allset);

    for( ; ; ){
        rset = allset;
        
        nready = select(maxfd + 1, &rset, NULL, NULL, NULL);
        if(nready == -1){
            perror("select returned failure");
            exit(EXIT_FAILURE);
        }

        /* CHECK FOR NEW CLIENT CONNECTION */
        if(FD_ISSET(listenfd, &rset)){
            /* NEW CLIENT */
            nclients++;
            if(debug_value > 3){
                printf("DEBUG: New client connected! Woot!\n");
            }
            clilen = sizeof(cliaddr);
            connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &clilen);


            /* ADD THE NEW CLIENT to the list of clients */
            for(i = 0 ; i < MAX_CLIENTS ; i++){
                if(client[i] < 0){
                    client[i] = connfd;
                    break;
                }
            }

            /* ADD THE DESRIPTOR TO THE ALLSET */
            FD_SET(connfd, &allset);
            if(connfd > maxfd){
                maxfd = connfd;
            }
        }


        /* CHECK FOR IF A CLIENT HAS SENT US INFOMATION */
        for(i = 0 ; i <= MAX_CLIENTS; i++){
            sockfd = client[i];
            if(sockfd < 0){
                continue; //Keep looking for data 
            }

            /* Check to see if this file desc is one of the ready ones */
            if(FD_ISSET(sockfd, &rset)){
                if(debug_value > 3){
                    printf("DEBUG: Recived communication from client! \n");
                }

                memset(buf, 0, sizeof(buf));
                /* READ FROM FD */
                if((nbytes = read(sockfd, buf, sizeof(buf))) == 0){
                    /* CLIENT LOSED CONNECTION */
                    close(sockfd);
                    FD_CLR(sockfd, &allset);
                    client[i] = NOT_IN_USE;

                    /* DEBUG */
                    if(debug_value > 3){
                        printf("DEBUG: Client lossed connection! \n");
                    }
                    /*********/
                }

                /* SEND RESPONSE BACK TO CLIENT */
                if((nbytes =write(sockfd, buf, nbytes)) == 0){

                }
                
            }
        }
    }
}



/* MAIN */
int main(int argc, char *argv[]){
    int opt;
    int port = PORT_NUM;

    memset((void *) &flags, 0, sizeof(flags_t));

    /* INIT FUNCTIONS */
    umask(0);

    /* DEBUG MESG TEMPLATE */
    if(debug_value > 3 ){
        printf("DEBUG: TEMPLATE DEBUG\n");
    }

    /* AT EXIT AND SIGNAL HANDLERS */
    atexit(onexit_function);
    signal(SIGINT, signal_handler);
    signal(SIGHUP, signal_handler);     //SIGHUP because osu-wifi sucks 
    
    
    while((opt=getopt(argc, argv, "p:hv")) != -1){
        switch(opt){
            case 'v':
                flags.flag_v = 1;
                break;
            case 'p': 
                if(debug_value > 0){
                    printf("DEBUG: PORT# = %s\n", optarg);
                }
                port = atoi(optarg);
                break;
            case 'h':
                showHelp();
                exit(EXIT_SUCCESS); // SHOUlDN"T GET HERE 
        }

    }

    /* CREATE SERVER PORT */

    /* RECIVE MESSAGES */
    createIPV4(port);
    //recMsgs();


    return 0;
}
