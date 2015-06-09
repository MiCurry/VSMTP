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
int debug_value = 0;

#include "statusCodes.h"
#include "header.h"

# define SERVER_CONFIRM "CONFIRMED"
# define CREATE_USER_NAME "add"

/* RAD-ASS FLAG STRUCT */
typedef struct flags_s{ 
    unsigned int flag_v : 1;    // If 1 produces verbose output
    unsigned int no_pl_flag : 1;
}flags_t;


flags_t flags;

static sig_atomic_t signal_recived = FALSE;
void signal_handler(int signum){signal_recived = TRUE; exit(EXIT_FAILURE);}
void onexit_function(void){;}




int connectIP(int port, char ip[100]){
    struct sockaddr_in sa;
    int sockfd;

    inet_pton(AF_INET, ip, &sa.sin_addr.s_addr);
    sa.sin_port = htons(port);
    sa.sin_family = AF_INET;

    /* SOCKET CREATION */
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

void r_msg(message_t msg, int sockfd){
    int nbytes;

    nbytes = read(sockfd, (char *) &msg, sizeof(message_t));

    if(debug_value > 0){
        printf("DEBUG: Message received from server!\n");
        printMessageHead(&msg, 1);
    }
}

void sendMsg(message_t msg){
    int numbytes;
    int sockfd = -1;

    sockfd = connectIP(PORT_NUM, FLIP1);

    numbytes = write(sockfd, (char *) &msg, sizeof(message_t));
    if(numbytes == -1){
        perror("Write Error");
        exit(EXIT_FAILURE);
    }
    if(debug_value > 0){
        printf("DEBUG: Message sent successfully!\n");
    }
    
    r_msg(msg, sockfd);
}


void init(void){
    memset((void *) &flags, 0, sizeof(flags_t));
    umask(0);
    atexit(onexit_function);
    signal(SIGINT, signal_handler);
    signal(SIGHUP, signal_handler);     
    pthread_exit((void *) pthread_self());
}

/*** Shell Commands ***/
int help(void){
    if(debug_value > 0){
        printf("DEBUG: client_help\n");
    }
    return 1;
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
    message_t msg_1;
  	char statCode[STATUS_CODE_LEN+1];
	char source[IP_CHAR_LEN];
	char dest[IP_CHAR_LEN];
	char from[MAX_USER_NAME];
	char to[MAX_USER_NAME];
	char messageBody[MAX_MESSAGE_LEN];

   if(debug_value > 0){
        printf("DEBUG: client_send\n>>>");
    	printf("Please enter your status code \n>>>");
        fgets(statCode, sizeof(statCode), stdin);
		printf("Please enter your ip \n>>>");
        fgets(source, sizeof(source), stdin);
		printf("Please enter the ip of your destination \n>>>");
        fgets(dest, sizeof(dest), stdin);
	}
	else{
		strcpy(statCode, "300");
		strcpy(source, "1.1.1.1");
		strcpy(dest, "2.2.2.2");
	}	
	printf("Please enter who the message is from \n>>>");
    fgets(from, sizeof(from), stdin);
	printf("Please enter who the message is to \n>>>");
    fgets(to, sizeof(to), stdin);
	printf("Please enter your message \n>>>");
    fgets(messageBody, sizeof(messageBody), stdin);
	fillMessageHeader(&msg_1, statCode, source, dest, from, to, messageBody);
	printMessageHead(&msg_1, 1);
    sendMsg(msg_1);

//    fillMessageHeader(&msg_1, "300", "1.1.1.1", "2.2.2.2", "Miles", "Jessica", "This is my message!");

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

    char buffer[MAX_SIZE];
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


int main(int argc, char **argv, char **envp){
    int opt;

    memset((void *) &flags, 0, sizeof(flags_t));

    /* INIT FUNCTIONS */
    {
        umask(0);
        if(debug_value > 0 ){
            printf("DEBUG: Debugger set to debug_value = %d \n", debug_value);
        }
        atexit(onexit_function);
        signal(SIGINT, signal_handler);
        signal(SIGHUP, signal_handler);     
    }

    while((opt=getopt(argc, argv, "I:P:H:hv")) != -1){
        switch(opt){
            case 'v':
                flags.flag_v = 1;
                break;
            case 'I':
                if(debug_value > 0){
                    printf("DEBUG: IP Addr = %s\n", optarg);
                }
                //strcpy(ip, optarg);
                break;
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

    pthread_t init_t; 
    pthread_create(&init_t, NULL, (void *) init, NULL);

    shell();

    //message_t msg_1;
   // message_t msg_2;
    message_t msg_1;
    message_t msg_2;

    fillMessageHeader(&msg_1, "300", "1.1.1.1", "2.2.2.2", "Miles", "Jessica", "This is my message!");
    printMessageHead(&msg_1, 1);
    sendMsg(msg_1);

    fillMessageHeader(&msg_2, MSG_TYPE_CMD, "0.0.0.0", FLIP1, "Miles", "Jessica", "A Message to flip!");
    printMessageHead(&msg_2, 1);
    sendMsg(msg_2);
    
    shell();

    return 1;
}
