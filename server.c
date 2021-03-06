# include "header.h"

# define BUF_SIZE 256
# define LISTENQ 1024
# define MAXLINE 4096

# define MAX_CLIENTS FD_SETSIZE
# define NOT_IN_USE -1

# define SERVER_CONFIRM "CONFIRMED"
# define PATH "~/Public/userNames"


int debug_value = 1;



typedef struct flags_s{ 
    unsigned int flag_v : 1;    // Produces verbose output
}flags_t;
flags_t flags;


static sig_atomic_t signal_recived = FALSE;
static void signal_handler(int signum){ signal_recived = TRUE; exit(EXIT_SUCCESS);}
static void onexit_function(void){}

int sendMsg(message_t msg, int sockFD){
    int numbytes;
    
    printf("DEBUG: Client FD: %d\n", sockFD);

    numbytes = write(sockFD, (char *) &msg, sizeof(message_t));
    if(numbytes == -1){
        perror("Write Error");
    }
    if(debug_value > 0){
        printMessageHead(&msg, 1);
        printf("DEBUG: Message sent successfully!\n");
    }
    return 1;
}

int reciveMsg(message_t msg_r, int sockFD){ 
    if(debug_value > 0){
        printMessageHead(&msg_r, 1);
    }

    sendMsg(msg_r, sockFD);
    return 1;
}

int addUser(char userName[MAX_USER_NAME]){
    if(debug_value > 0){
        printf("DEBUG: addUser function\n");
    }
    return 1;
}

int serverInit(int port){
        /* Create File for userNames list */
    {
        char *name = "userNames";

        if(-1 == access(name, F_OK)){
            if(createFile(name) == 0){
               if(debug_value > 0){
                    printf("DEBUG:  Could not create userName file %s\n", name);
               }
                exit(EXIT_FAILURE);
            }; 
            if(debug_value > 0){
                printf("DEBUG: Username file created\n");
            }
        }
        if(debug_value > 0){
            printf("DEBUG: Username file already exists\n");
        }
    }

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

    struct sockaddr_in servaddr;   
    struct sockaddr_in cliaddr;

    message_t msg;

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

                /* READ FROM FD */
                if((nbytes = read(sockfd, (char *) &msg, sizeof(message_t))) == 0){
                    /* CLIENT LOST CONNECTION */
                    close(sockfd);
                    FD_CLR(sockfd, &allset);
                    client[i] = NOT_IN_USE;

                    if(debug_value > 3){
                        printf("DEBUG: Client lossed connection! \n");
                    }
                }
                reciveMsg(msg, sockfd);
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
                exit(EXIT_SUCCESS); // SHOUlDN"T GET HERE 
        }

    }

    /* CREATE SERVER PORT */

    /* RECIVE MESSAGES */
    serverInit(port);

    //recMsgs();


    return 0;
}
