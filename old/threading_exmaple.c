/* Miles A. Curry
 * currymi@onid.orst.edu
 * CS344-001
 * Assignment #4 
 */

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

#include "posixmsg.h"
#include "server.h"

#define TRUE 1
#define FALSE 0
#define BUF_SIZE 256
#define MAX_SIZE sizeof(message_t)

char server_dir[PATH_MAX];

int parent_PID;
int debug_value = 0;
int mq;

char sMsgQName[BUF_SIZE];

static sig_atomic_t signal_recived = FALSE;
static pthread_mutex_t mutex_client;

/* Signal Handler's & On Exit Functions */
static void signal_handler(int signum){
    signal_recived = TRUE;
    exit(EXIT_SUCCESS);
}

/* Cleans up the mq at exit */
static void onexit_function(void){
    if(mq_unlink(sMsgQName) != 0){
        perror("Failed in mq_unlink()");
    }
}


/***** SERVER COMMANDS START *****/
static void exitCmd(void){
    if(debug_value > 1){
        printf("DEBUG: exitCmd called succesfully\n");
    }

    pthread_detach(pthread_self());

}

int cdCmd(message_t msg, int reader_fd){
    message_t msg_s;
    memset(&msg_s, 0, MAX_SIZE);

    if(-1 == chdir(msg.payload)){        
        perror("Failed to change servers' directory");
    }

    sprintf(msg_s.command, "%s", "Servers CWD:");
    getcwd(msg_s.payload, PATH_MAX); //Update the threads 'directory'
    msg_s.message_type = MESSAGE_TYPE_NORMAL;
   
    if(0 != mq_send(reader_fd, (char *) &msg_s, sizeof(msg), 1)){
        perror("Faid to send message to");
        exit(EXIT_FAILURE);
    }

    if(debug_value > 1){
        printf("DEBUG: cd command called succesfully\n");
    }

    return 0;
}

int dirCmd(int reader_fd){
    FILE *fp;
    char path[PATH_MAX];
    message_t msg;

    /* FIRST DIR INIT MESSAGE */
    memset(&msg, 0, MAX_SIZE);
    msg.message_type = MESSAGE_TYPE_DIR;


    /* READ THE DIRECTORY AND SEND IT */
    fp = popen(CMD_LS_POPEN, "r");
    if(fp == NULL){
        perror("Popen error: ");
        return 0;
    }
    while(fgets(path, PATH_MAX, fp) != NULL){
        memset(&msg, 0, MAX_SIZE);
        msg.message_type = MESSAGE_TYPE_DIR;
        sprintf(msg.payload, "%s", path);

        if(0 != mq_send(reader_fd, (char *) &msg, sizeof(msg), 1)){
            perror("Faid to send message to");
            exit(EXIT_FAILURE);
        }
   
    if(debug_value > 1){
        printf("DEBUG: client ldir called succesfully\n");
    }

    /* CLEAR BUFFER */
    memset(&msg, 0, MAX_SIZE);

    /* CREATE DIR END MESSAGE */
    msg.message_type = MESSAGE_TYPE_DIR_END;
    if(0 != mq_send(reader_fd, (char *) &msg, sizeof(msg), 1)){
        perror("Faid to send message to");
        exit(EXIT_FAILURE);
    }

    pclose(fp);
    return 0;
}

int pwdCmd(int reader_fd){
    message_t msg_s;
    memset(&msg_s, 0, MAX_SIZE);

    getcwd(msg_s.payload, PATH_MAX); //Update the threads 'directory'
    msg_s.message_type = MESSAGE_TYPE_NORMAL;

    if(0 != mq_send(reader_fd, (char *) &msg_s, sizeof(msg_s), 1)){
        perror("Faid to send message to");
        exit(EXIT_FAILURE);
    }
    if(debug_value > 1){
        printf("DEBUG: pwd command called succesfully\n");
    }
    return 0;
}

int homeCmd(int reader_fd){
    message_t msg_s;
    memset(&msg_s, 0, MAX_SIZE);

    if(debug_value > 1){
        printf("DEBUG: home commmand called succesfully\n");
    }

    if(-1 == chdir(getenv("HOME"))){
        perror("Failed to chdir");
    }

    sprintf(msg_s.command, "%s\n", "Server now at home:");
    getcwd(msg_s.payload, PATH_MAX); //Update the threads 'directory'
    msg_s.message_type = MESSAGE_TYPE_NORMAL;

    if(0 != mq_send(reader_fd, (char *) &msg_s, sizeof(msg_s), 1)){
        perror("Faid to send message to");
        exit(EXIT_FAILURE);
    }
    if(0 != mq_send(reader_fd, (char *) &msg_s, sizeof(msg_s), 1)){
        perror("Faid to send message to");
        exit(EXIT_FAILURE);
    }

    return 0;
}


static void put(message_t msg,int reader_fd,int writer_fd){ 
    int fd;
    int num_byte;
    int bytes_read;
    message_t msg_s;

    if(debug_value > 1){
        printf("DEBUG: Put called succesfully\n");
    }

    /*OPEN READ*/
    fd = open(msg.payload, SEND_FILE_FLAGS, SEND_FILE_PERMISSIONS);
    while(1){
        /* Read the payload */
        bytes_read = mq_receive(writer_fd, (char *) &msg_s, MAX_SIZE, NULL);
        if(bytes_read < 0){
            perror("Failed read");
            return;
        }
        /* If were done then break */
        if(msg_s.message_type == MESSAGE_TYPE_SEND_END){
            break;
        }
        /* Write the Payload */
        printf("%d\n", msg_s.num_bytes);
        num_byte = write(fd, msg_s.payload, msg_s.num_bytes);
        if(num_byte < 0){
            perror("Failed read");
            exit(EXIT_FAILURE);
        }

    }
}


static void getCmd(message_t msg, int reader_fd,int writer_fd){ 
    char buffer[MAX_SIZE];
    int fd;
    int bytes_read;
    message_t msg_s;

    memset(&msg_s, 0, sizeof(msg_s));

    if(debug_value > 1){
        printf("DEBUG: client put called succesfully\n");
    }

    /* CHECK IF FILE EXISTS ON CLIENT SIDE */
    if(access(msg.payload, F_OK) == -1){
        perror("ERROR: No such file");
        return;
    }

    /* READ THE FILE AND SEND IT */
    fd = open(msg.payload, O_RDONLY);
    while(1){
        msg_s.num_bytes = read(fd, msg_s.payload, PAYLOAD_LENGTH);
        if(msg_s.num_bytes == -1){
            perror("Failed read");
            return;
        }
        printf("%d\n", msg_s.num_bytes);
        if(msg_s.num_bytes == 0){
            break;
        }
        msg_s.message_type = MESSAGE_TYPE_SEND;
        bytes_read = mq_send(reader_fd, (char *) &msg_s, MAX_SIZE, 1);
        if(bytes_read < 0){
            perror("Failed read");
            exit(EXIT_FAILURE);
        }

    }
    
    /* SEND THE END */
    memset(&buffer, '0', MAX_SIZE + 1);
    msg.message_type = MESSAGE_TYPE_SEND_END;
    if(0 != mq_send(reader_fd, (char *) &msg, MAX_SIZE, 1)){
        perror("Faid to send shell message");
        exit(EXIT_FAILURE);
    }

}



/***** SERVER COMMANDS END  *****/


/***** START SERVER/CLIENT COMM *****/
static void *reciveMsg(void * pid_mesg){
        /* CREATING THE PID */
        int pid = *((int *) pid_mesg);

        char reader_buffer[BUF_SIZE];
        char writer_buffer[BUF_SIZE];
        char thread_dir[BUF_SIZE];
        char buffer[MAX_SIZE + 1];

        char checker[PATH_MAX];

        message_t msg;

        int bytes_read;
        int reader_fd;
        int writer_fd;

        /* Get thread init directory */
        getcwd(thread_dir, PATH_MAX);

        /**** CLEAR BUFFERS FOR GOOD LUCK ****/
        memset(&reader_buffer, 0, BUF_SIZE);
        memset(&writer_buffer, 0, BUF_SIZE);
        
        if(debug_value > 1){
            printf("DEBUG: pThread creation successfull\n");
            printf("DEBUG: Server child thread waiting for client messages \n");
        }

        /*** OPENING CLIENT MESSAGE QUEUES ***/
              /**** client WRITES to WRITER  */
              /**** client READS from READER */
              /**** server READS from WRITER */
              /**** server WRITES to READER  */ 

        /* SERVER */
        CREATE_CLIENT_WRITER_NAME(writer_buffer, pid);
        writer_fd = mq_open(writer_buffer, O_RDONLY);
        if(writer_fd == -1){
            perror("Failed to open client writer message queue:");
        }
        if(debug_value > 1){
            printf("DEBUG: Server connected to  %s successfull \n", writer_buffer);
        }
        
        /* READER */
        CREATE_CLIENT_READER_NAME(reader_buffer, pid);
        reader_fd = mq_open(reader_buffer, O_WRONLY);
        if(reader_fd == -1){
            perror("Failed to open client reader message queue:");
            exit(EXIT_FAILURE);
        }
        if(debug_value > 1){
            printf("DEBUG: Server connected to %s successfull\n", reader_buffer);
        }

        /* RECIVING MESSAGES */
        while(1){
            memset(&buffer, '0', MAX_SIZE + 1);
            memset(&msg, 0, MAX_SIZE);
            bytes_read = 0;

            /* READ FROM THE CLIENT'S MESSAGE QUEUE */
            bytes_read = mq_receive(writer_fd, buffer, sizeof(msg), NULL);
            if(bytes_read < 0){
                perror("Failed read");
                exit(EXIT_FAILURE);
            }

            /* DEBUG */        
                if(debug_value > 0){
                    printf("DEBUG: Message recived!\n");
                }
            /********/

            /* CONSTRUCT MESSAGE FROM RECIVED BUFFER */
            memcpy(&msg, buffer, MAX_SIZE + 1);

            /* DEBUG */        
            if(debug_value > 1){
                printf("MT: %d \n", msg.message_type);
                printf("MC: %s \n", msg.command);
                printf("PL: %s \n", msg.payload);
                printf("NB: %d \n", msg.num_bytes);
            }
            /********/


            /************ MESSAGE DECIFER ******************/
            /* Lock all other threads from this point on */
            pthread_mutex_lock(&mutex_client);

            /* CHANGE THE PROCCESS DIRECTORY TO THE THREADS 'DIRECTORY' */
            /*  This will be changed back after our thread finishes with the
             *  message */
            /*  This is why we need the lock */
            if(-1 == chdir(thread_dir)){        // This is gonna be changed back after 
                perror("Failed to change to threads directory");
            }
            /* DEBUG: Check cwd */        
            /********/

            /* EXIT */
            if(strncmp(msg.command, CMD_EXIT, strlen(CMD_EXIT)) == 0){
                exitCmd();

            }
            /* REMOTE CHDIR */
            else if(strncmp(msg.command, CMD_REMOTE_CHDIR, strlen(CMD_REMOTE_CHDIR)) == 0){
                cdCmd(msg, reader_fd);

            }
            /* REMOTE DIR/LS */
            else if(strncmp(msg.command, CMD_REMOTE_DIR, strlen(CMD_REMOTE_DIR)) == 0){
                dirCmd(reader_fd);

            }
            /* REMOTE PWD*/
            else if(strncmp(msg.command, CMD_REMOTE_PWD, strlen(CMD_REMOTE_PWD)) == 0){
                pwdCmd(reader_fd);
            }
            /* REMOTE HOME */
            else if(strncmp(msg.command, CMD_REMOTE_HOME, strlen(CMD_REMOTE_HOME)) == 0){
                homeCmd(reader_fd);
            }
            /* GET */
            else if(strncmp(msg.command, CMD_GET, strlen(CMD_GET)) == 0){
                getCmd(msg, reader_fd, writer_fd);    
            }else if(strncmp(msg.command, CMD_PUT, strlen(CMD_PUT)) == 0){
                put(msg, reader_fd, writer_fd);   
            }
            else{
                printf("Could not recognize %s\n", msg.command);
                printf("Type 'Help' for a list of commands\n");
            }
            
            getcwd(thread_dir, PATH_MAX); //Update the threads 'directory'

            /* Change the proccess's directory back to what it should be */
            if(-1 == chdir(server_dir)){        
                perror("Failed to change back to proccess' directory");
            }
            /* DEBUG: Check directory */        
            if(debug_value > 1){
                getcwd(checker, PATH_MAX);
                printf("%s\n", checker);
            }
            /********/

            /* Unlock */
            pthread_mutex_unlock(&mutex_client);

        }
}


int recivePID(void){
    int bytes_read;
    char buffer[BUF_SIZE];
    int pid;
    message_t msg;
    pthread_t thread;
    
    while(1){
        memset(buffer, 0, BUF_SIZE);
        memset(&msg,  0, sizeof(msg));

        bytes_read = mq_receive(mq, (char *) &msg, sizeof(msg), NULL);

        if(bytes_read < 0){
            perror("Failed read");
            exit(EXIT_FAILURE);
        }

        pid = atoi(msg.command);

        if(debug_value > 0){
            printf("PID recived\n");
            printf("PID = %d\n", pid);
        }

        if(0 != pthread_create(&thread, NULL, (void *) reciveMsg,(void *) &pid)){
            perror("Failed thread creation");
            exit(EXIT_FAILURE);
        }
        
        if(debug_value > 1){
            printf("DEBUG: parent thread continue to wait for more pids \n");
        }
    }
}

int init_posMsq(void){
    struct mq_attr attr;

    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MAX_SIZE;
    attr.mq_curmsgs = 0;

    CREATE_SERVER_QUEUE_NAME(sMsgQName);
    printf("%s\n", sMsgQName);
    mq = mq_open(sMsgQName, O_CREAT | O_RDONLY, QUEUE_PERMISSIONS, &attr);
    if(mq == -1){
        perror("Failed to create server message queue:");
        exit(EXIT_FAILURE);
    }
    return 1;
}

int main(int argc, char **argv, char **envp){


    /**** INITAL FUNCTIONS ****/
    {
        umask(0);

        memset(server_dir, 0, sizeof(server_dir));
        
        getcwd(server_dir, PATH_MAX);

        /* DEBUG */
        if(debug_value > 0 ){
            printf("%s\n", server_dir);
        }

        parent_PID = getpid();

        /* AT EXIT AND SIGNAL HANDLERS */
        atexit(onexit_function);
        signal(SIGINT, signal_handler);
        signal(SIGHUP, signal_handler);     //SIGHUP because osu-wifi sucks 

        /* Init Mutex */
        pthread_mutex_init(&mutex_client, NULL);
    }

    /* CREATE SERVER MESSAGE QUEUE */
    init_posMsq();

    /* WAIT FOR A CLIENT CONNECTION PID */
    recivePID();

    return 0;
}
