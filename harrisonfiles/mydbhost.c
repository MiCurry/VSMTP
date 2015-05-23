/*************************************************************
 * Name: Harrison Alan Kaiser
 * Email: kaiserh@onid.oregonstate.edu
 * Class: CS344-001
 * Assignment #5
 * References: Class Materials
 * TLPI
 * **********************************************************/
//LIBS
#include <stdio.h>
#include <linux/limits.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <libgen.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <time.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <semaphore.h>
//LOCAL LIBS
#include "hostdb.h"
//DEFINITIONS
#define DEBUG
typedef int bool;
enum { false, true };
//GLOBALS
static sig_atomic_t sig_here = false;
static char smu[NAME_SIZE];
static int smfd;
static void* smaddr;
//static void* addr;
static struct host_row_s* hrtaddr;
static int shared_size;
static int db_lock = 0;
static int rlock = 0;
static int rowned;
//STRUCTS
//PROTOTYPES
void sig_handler(int);
void rem_sm(void);
void rem_sm_light(void);
//
int main(int agrc, char* argv[], char* envp[]){
//DECLARATIONS
    char cmd[NAME_SIZE];
    char buff[PATH_MAX];
    //host_row_t *ent;
	//sem_t *sems[MAX_ROWS];
	sem_t *db_sem;
    
	char* input = NULL;
    int i = 0;
	
	int check = 0;
	//char hostname[NI_MAXHOST];
	char ip_s[INET6_ADDRSTRLEN];
	struct addrinfo store;
	struct addrinfo *alist;
	struct addrinfo *a;
	time_t raw;
	char db_prompt[NAME_SIZE];
	char* dbp = "DBlock";
	char* rlp = "Rlock";
	//char row_prompt[NAME_SIZE];
	
//MEMORY MANAGEMENT
    umask(0);
	atexit(rem_sm);
    memset(buff, 0, sizeof(buff));
	strcpy(db_prompt, PROMPT);
//SIGNALS
    signal(SIGINT, sig_handler);
    signal(SIGPIPE, sig_handler);
//CREATE SHARED MEMORY
    SHARED_MEM_NAME(smu);		
    smfd = shm_open(smu, O_CREAT | O_EXCL | O_RDWR, SHARED_MEM_PERMISSIONS);
    if (smfd < 0){
        smfd = shm_open(smu, O_RDWR, SHARED_MEM_PERMISSIONS);
    }
    shared_size = sizeof(sem_t) + (MAX_ROWS * sizeof(host_row_t));
	ftruncate(smfd, shared_size);
    smaddr = mmap(NULL, shared_size, PROT_READ | PROT_WRITE, MAP_SHARED, smfd, 0);
	if (smaddr == NULL){
		perror("mmap()");
		exit(EXIT_FAILURE);
	}	
//INITIALIZE SEMAPHORES
	db_sem = (sem_t*) smaddr;
	sem_init(db_sem, PSHARED_MULTIPROCESS, PSHARED_MULTIPROCESS);
	hrtaddr =  (struct host_row_s*) (smaddr + (sizeof(sem_t)));
	for(; i < MAX_ROWS; i++){
        //sems[i] = (sem_t*) addr;
		sem_init(&hrtaddr[i].host_lock, PSHARED_MULTIPROCESS, PSHARED_MULTIPROCESS);
		//addr = addr+(sizeof(host_row_t));
    }
//COMMAND LOOP
    while(!sig_here){
        memset(cmd, 0, sizeof(cmd));
        memset(buff, 0, sizeof(buff));
		if (db_lock == 1){
			strcpy(db_prompt, dbp);
			strcat(db_prompt, PROMPT);
		}
		else if(rlock == 1){
			strcpy(db_prompt, rlp);
			sprintf(buff, "%d ", rowned);
			strcat(db_prompt, buff);
			strcat(db_prompt, PROMPT);		
		}
		else{
			strcpy(db_prompt, PROMPT);
		}
    //PROMPT
        fputs(db_prompt, stdout);
        fflush(stdout);
        input = fgets(cmd, sizeof(cmd), stdin);
        if (input == NULL || sig_here == true)
            break;
        if ((input = strchr(cmd, '\n')) != NULL)
            *input = '\0';
        if ((strncmp(cmd, CMD_HELP, 4)) == 0){
            printf("Listing of Client Commands:\n");
            printf("\thelp\t\t: You are Here!\n");
        }
        else if ((strncmp(cmd, CMD_INSERT, strlen(CMD_INSERT))) == 0){
        //INSERT
			int count = 0;
			for(i = 0;i < (strlen(cmd)); i++){
				if ( i >= strlen(cmd)-(strlen(CMD_INSERT))){
					cmd[i] = '\0';
					break;
				}
				else
					cmd[i] = cmd[i+(strlen(CMD_INSERT))];
			}
			i = 0;
			hrtaddr = (struct host_row_s*) (smaddr + sizeof(sem_t));
			sem_wait(db_sem);
			while(strlen(hrtaddr[i].host_name) > 0){
				if (strcmp(hrtaddr[i].host_name, cmd) == 0){
					printf("Hostname currently in DB\n");
					printf("Name: %s\n", hrtaddr[i].host_name);
					printf("\tIPv4: %s\n", hrtaddr[i].host_address_ipv4);
					printf("\tIPv6: %s\n", hrtaddr[i].host_address_ipv6);
					printf("\tTime fetched: %s", ctime(&(hrtaddr[i].time_when_fetched)));
					count++;
					break;
				}
				i++;
			}
			if (count == 0){
			sem_wait(&hrtaddr[i].host_lock);
			memset(&store, 0, sizeof(store));
			store.ai_family = AF_UNSPEC;
			store.ai_socktype = SOCK_STREAM;
			check = getaddrinfo(cmd, NULL, &store, &alist);
			if (check != 0){
				fprintf(stderr, "ADDR_INFO: %s\n", gai_strerror(check));
			}
			sprintf(hrtaddr[i].host_name, "%s", cmd);
			for (a = alist; a != NULL; a = a->ai_next){
				void* add;
				if (a->ai_family == AF_INET){  //IPv4
					if (count == 0){
						struct sockaddr_in *ipv4 = (struct sockaddr_in*) a->ai_addr;
						add = &(ipv4->sin_addr);
						inet_ntop(a->ai_family, add, ip_s, sizeof ip_s);
						sprintf(hrtaddr[i].host_address_ipv4, "%s", ip_s);
						count++;
					}
				}
				else{  //IPv6
					struct sockaddr_in6 *ipv6 = (struct sockaddr_in6*) a->ai_addr;
					add = &(ipv6->sin6_addr);
					inet_ntop(a->ai_family, add, ip_s, sizeof(ip_s));
					sprintf(hrtaddr[i].host_address_ipv6, "%s", ip_s);					
				}
			}
			time(&raw);
			hrtaddr[i].time_when_fetched = raw;		
			freeaddrinfo(alist);
			printf("Name: %s\n", hrtaddr[i].host_name);
			printf("\tIPv4: %s\n", hrtaddr[i].host_address_ipv4);
			printf("\tIPv6: %s\n", hrtaddr[i].host_address_ipv6);
			printf("\tTime fetched: %s", ctime(&(hrtaddr[i].time_when_fetched)));			
			sem_post(&hrtaddr[i].host_lock);
			}
			sem_post(db_sem);
        }
        else if ((strncmp(cmd, CMD_UPDATE, strlen(CMD_UPDATE))) == 0){
        //UPDATE
			int count = 0;
			for(i = 0;i < (strlen(cmd)); i++){
				if ( i >= strlen(cmd)-(strlen(CMD_UPDATE))){
					cmd[i] = '\0';
					break;
				}
				else
					cmd[i] = cmd[i+(strlen(CMD_UPDATE))];
			}
			i = 0;
			hrtaddr = (struct host_row_s*) (smaddr + sizeof(sem_t));
			sem_wait(db_sem);
			while(strlen(hrtaddr[i].host_name) > 0){
				if (strcmp(hrtaddr[i].host_name, cmd) == 0){
					break;
				}
				i++;
			}
			if((strlen(hrtaddr[i].host_name) == 0) && (strlen(cmd) > 0)){
				printf("Hostname %s not in Database.\n", cmd);
			}
			else{
				sem_wait(&hrtaddr[i].host_lock);
				memset(&store, 0, sizeof(store));
				store.ai_family = AF_UNSPEC;
				store.ai_socktype = SOCK_STREAM;
				check = getaddrinfo(cmd, NULL, &store, &alist);
				if (check != 0){
					fprintf(stderr, "ADDR_INFO: %s\n", gai_strerror(check));
				}
				sprintf(hrtaddr[i].host_name, "%s", cmd);
				for (a = alist; a != NULL; a = a->ai_next){
					void* add;
					if (a->ai_family == AF_INET){  //IPv4
						if (count == 0){
							struct sockaddr_in *ipv4 = (struct sockaddr_in*) a->ai_addr;
							add = &(ipv4->sin_addr);
							inet_ntop(a->ai_family, add, ip_s, sizeof ip_s);
							sprintf(hrtaddr[i].host_address_ipv4, "%s", ip_s);
							count++;
						}
					}
					else{  //IPv6
						struct sockaddr_in6 *ipv6 = (struct sockaddr_in6*) a->ai_addr;
						add = &(ipv6->sin6_addr);
						inet_ntop(a->ai_family, add, ip_s, sizeof(ip_s));
						sprintf(hrtaddr[i].host_address_ipv6, "%s", ip_s);					
					}
				}
				time(&raw);
				hrtaddr[i].time_when_fetched = raw;		
				freeaddrinfo(alist);
				printf("Name: %s\n", hrtaddr[i].host_name);
				printf("\tIPv4: %s\n", hrtaddr[i].host_address_ipv4);
				printf("\tIPv6: %s\n", hrtaddr[i].host_address_ipv6);
				printf("\tTime fetched: %s", ctime(&(hrtaddr[i].time_when_fetched)));			
				sem_post(&hrtaddr[i].host_lock);
			}
			sem_post(db_sem);            
        }
        else if ((strncmp(cmd, CMD_SELECT, strlen(CMD_SELECT))) == 0){
        //SELECT
			int count = 0;
			for(i = 0;i < (strlen(cmd)); i++){
				if ( i >= strlen(cmd)-(strlen(CMD_SELECT))){
					cmd[i] = '\0';
					break;
				}
				else
					cmd[i] = cmd[i+(strlen(CMD_SELECT))];
			}
			hrtaddr = (struct host_row_s*) (smaddr + sizeof(sem_t));
			sem_wait(db_sem);
			while(strlen(hrtaddr[i].host_name) != 0){
				if (strcmp(hrtaddr[i].host_name, cmd) == 0){
					printf("Hostname currently in DB\n");
					printf("Name: %s\n", hrtaddr[i].host_name);
					printf("\tIPv4: %s\n", hrtaddr[i].host_address_ipv4);
					printf("\tIPv6: %s\n", hrtaddr[i].host_address_ipv6);
					printf("\tTime fetched: %s", ctime(&(hrtaddr[i].time_when_fetched)));
					count++;
					break;
				}
				i++;
			}
			if (count == 0){
				for(i = 0; i < MAX_ROWS; i++){
					if(strlen(hrtaddr[i].host_name) > 1){
						//printf("%d:%s:\n",(int)(strlen(hrtaddr[i].host_name)),hrtaddr[i].host_name);
						printf("Name: %s\n", hrtaddr[i].host_name);
						printf("\tIPv4: %s\n", hrtaddr[i].host_address_ipv4);
						printf("\tIPv6: %s\n", hrtaddr[i].host_address_ipv6);
						printf("\tTime fetched: %s", ctime(&(hrtaddr[i].time_when_fetched)));			
						count++;
					}					
				}
				if((i == MAX_ROWS) && (strlen(cmd) > 0)){
					printf("Hostname %s not in Database.\n", cmd);
				}
			}
			printf("Rows Selected: %d\n", count);
			sem_post(db_sem);
        }
        else if ((strncmp(cmd, CMD_DELETE, strlen(CMD_DELETE))) == 0){
        //DELETE
			for(i = 0;i < (strlen(cmd)); i++){
				if ( i >= strlen(cmd)-(strlen(CMD_DELETE)+1)){
					cmd[i] = '\0';
					break;
				}
				else
					cmd[i] = cmd[i+(strlen(CMD_DELETE)+1)];
			}
			hrtaddr = (struct host_row_s*) (smaddr + sizeof(sem_t));
			sem_wait(db_sem);
			if(strlen(cmd) == 0){
				for(i = 0; i < MAX_ROWS; i++){
					memset(hrtaddr[i].host_name, 0, sizeof(hrtaddr[i].host_name));
					memset(hrtaddr[i].host_address_ipv4, 0, sizeof(hrtaddr[i].host_address_ipv4));
					memset(hrtaddr[i].host_address_ipv6, 0, sizeof(hrtaddr[i].host_address_ipv6));
					hrtaddr[i].time_when_fetched = 0;
				}
			}
			i = 0;
			//printf(":%s:\n", cmd);
			while(strlen(hrtaddr[i].host_name) != 0){
				if (strcmp(hrtaddr[i].host_name, cmd) == 0){
					sem_wait(&hrtaddr[i].host_lock);
					memset(hrtaddr[i].host_name, 0, sizeof(hrtaddr[i].host_name));
					memset(hrtaddr[i].host_address_ipv4, 0, sizeof(hrtaddr[i].host_address_ipv4));
					memset(hrtaddr[i].host_address_ipv6, 0, sizeof(hrtaddr[i].host_address_ipv6));
					hrtaddr[i].time_when_fetched = 0;
					cmd[0] = '\0';
					sem_post(&hrtaddr[i].host_lock);
					break;
				}
				i++;
			}
			if((strlen(hrtaddr[i].host_name) == 0) && (strlen(cmd) > 0)){
				printf("Hostname %s not in Database.\n", cmd);
			}			
			sem_post(db_sem);
        }
        else if ((strncmp(cmd, CMD_COUNT, strlen(CMD_COUNT))) == 0){
        //COUNT
            //GIVES ACTIVE ROW COUNT FOR DB
        }
        else if ((strncmp(cmd, CMD_SAVE, strlen(CMD_SAVE))) == 0){
        //SAVE
			int pd;
			for(i = 0;i < (strlen(cmd)); i++){
				if ( i >= strlen(cmd)-(strlen(CMD_SAVE))){
					cmd[i] = '\0';
					break;
				}
				else
					cmd[i] = cmd[i+(strlen(CMD_SAVE))];
			}
            pd = creat(cmd, SHARED_MEM_PERMISSIONS);
			while(strlen(hrtaddr[i].host_name) != 0){
				write(pd, hrtaddr[i].host_name, sizeof(hrtaddr[i].host_name));
			}
			close(pd);
			//PUSH ALL HOSTNAMES INTO FILE
            //LOCK ROWS AS YOU PULL NAMES
        }
        else if ((strncmp(cmd, CMD_LOAD, strlen(CMD_LOAD))) == 0){
        //LOAD
			for(i = 0;i < (strlen(cmd)); i++){
				if ( i >= strlen(cmd)-(strlen(CMD_LOAD))){
					cmd[i] = '\0';
					break;
				}
				else
					cmd[i] = cmd[i+(strlen(CMD_LOAD))];
			}
            //PULL ALL HOSTNAMES FROM FILE
            //FGETS SINCE ONE PER LINE
            //USE INSERT FOR EACH HOSTNAME
            //
        }
        else if ((strncmp(cmd, CMD_DROP_DB, strlen(CMD_DROP_DB))) == 0){
        //DROP
			printf("Database Dropped\n");
		    rem_sm();
        }
        else if ((strncmp(cmd, CMD_LOCK_DB, strlen(CMD_LOCK_DB))) == 0){
        //LOCK DB
			int x = 0;
			sem_getvalue(db_sem, &x);
			if((x > 0) && (db_lock == 0)){
				sem_wait(db_sem);
				db_lock = 1;
				printf("Locked Database.\n");
			}
			else if (((x) == 0) && db_lock == 1){
				printf("Cannot lock Database twice.\n");	
			}
			else{
				printf("Waiting for Database Lock...\n");
				sem_wait(db_sem);	
			}
        }
        else if ((strncmp(cmd, CMD_UNLOCK_DB, strlen(CMD_UNLOCK_DB))) == 0){
        //UNLOCK DB
			int x = 0;
			sem_getvalue(db_sem, &x);
			if((x > 0) && (db_lock == 0)){
				printf("Database is not locked. Cannot unlock.\n");
			}
			else if((x == 0) && (db_lock == 1)){
				printf("Unlocked Database.\n");
				sem_post(db_sem);
				db_lock = 0;
			}
			else{
				printf("Databased is locked on another client.\n");
			}
        }
        else if ((strncmp(cmd, CMD_LOCK_ROW, strlen(CMD_LOCK_ROW))) == 0){
        //LOCK ROW
			int x = 0;
			for(i = 0;i < (strlen(cmd)); i++){
				if ( i >= strlen(cmd)-(strlen(CMD_LOCK_ROW))){
					cmd[i] = '\0';
					break;
				}
				else
					cmd[i] = cmd[i+(strlen(CMD_LOCK_ROW))];
			}
			i = 0;
			hrtaddr = (struct host_row_s*) (smaddr + sizeof(sem_t));
			sem_wait(db_sem);
			while(strlen(hrtaddr[i].host_name) != 0){
				if (strcmp(hrtaddr[i].host_name, cmd) == 0){
					break;
				}
				i++;
			}
			if(strlen(hrtaddr[i].host_name) > 0){
				sem_getvalue(&hrtaddr[i].host_lock, &x);
				if((x > 0) && (rlock == 0)){
					sem_post(db_sem);
					sem_wait(&hrtaddr[i].host_lock);
					rlock = i+1;
					printf("Locked Row (%d) %s\n", i, hrtaddr[i].host_name);
				}
				else if (((x) == 0) && rlock > 0){
					printf("Cannot lock more than single row.\n");
					sem_post(db_sem);
				}
				else{
					printf("Waiting for Row Lock: %s...\n", hrtaddr[i].host_name);
					sem_post(db_sem);
					sem_wait(&hrtaddr[i].host_lock);
					rlock = i+1;
				}
			}
			sem_post(db_sem);
        }
        else if ((strncmp(cmd, CMD_UNLOCK_ROW, strlen(CMD_UNLOCK_ROW))) == 0){
        //UNLOCK ROW
			int x = 0;
			hrtaddr = (struct host_row_s*) (smaddr + sizeof(sem_t));
			sem_wait(db_sem);
			i = rlock-1;
			if(strlen(hrtaddr[i].host_name) > 0){
				sem_getvalue(&hrtaddr[i].host_lock, &x);
				if((x > 0) && (rlock == 0)){
					printf("Row (%d) %s is not locked. Cannot unlock.\n", i, hrtaddr[i].host_name);
				}
				else if((x == 0) && (rlock > 0)){
					printf("Unlocked Row (%d) %s\n", i, hrtaddr[i].host_name);
					sem_post(&hrtaddr[i].host_lock);
					rlock = 0;
				}
				else{
					printf("Row (%d) %s is locked on another client.\n", i, hrtaddr[i].host_name);
				}
			}				
			sem_post(db_sem);			
        }
        else if ((strncmp(cmd, CMD_LOCKS, strlen(CMD_LOCKS))) == 0){
        //LOCK LIST
        }
        else if ((strncmp(cmd, CMD_EXIT, strlen(CMD_EXIT))) == 0){
        //EXIT PROGRAM
            break;
        }
        else
            printf("COMMAND ERROR --> CMD_UNKNOWN\n");
    }
//EXIT CATCH
    atexit(rem_sm_light);
    return(EXIT_SUCCESS);
}
void sig_handler(int num){
    fprintf(stderr, "\nEXIT SIGNAL --> %d\n", num);
    sig_here = true;
    //NEED TO USE FLAG TO AVOID LOCKING ISSUE
    //rem_sm_light();
    exit(EXIT_FAILURE);
}
void rem_sm(void){
    munmap(smaddr, shared_size);
    close(smfd);
    shm_unlink(smu);
}
void rem_sm_light(void){
    munmap(smaddr, shared_size);
    close(smfd);
}