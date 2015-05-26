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
#include <ctype.h>
#include <string.h>
#include <strings.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>
#include <libgen.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/select.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
//LOCAL LIBS
#include "socket_hdr.h"
//DEFINES
#define CMD_OP "p"
//GLOBALS
static int listenfd;
static int sockfd;
static int client[MAX_CLIENTS];
static FILE* pofd;
//STRUCTS
//PROTOTYPES
void sig_handler(int);
void rm_soc(void);
//
int main (int argc, char* argv[], char* envp[]){
	//DECLARATIONS
	int i = 0;
	int k;
	int op = 0;
	int check;
	int pnum;
	char pnum_str[12];
	char buff[MAXLINE];
	char currdir[MAXLINE];
	char cmd[MAXLINE];
	char dir[MAX_CLIENTS][MAXLINE];
	int maxfd;
	int connfd;
	int tot_clients = 0;
	fd_set all;
	fd_set rset;
	
	
	struct sockaddr_in my_ad;
	struct sockaddr_in client_ad;
	socklen_t client_len;
	
	//MEMORY MANAGEMENT
	memset(&my_ad, 0, sizeof(my_ad));
	signal(SIGINT, sig_handler);
    signal(SIGPIPE, sig_handler);
	for (; i < MAX_CLIENTS; i++){
		client[i] = -1;
		getcwd(dir[i], sizeof(dir[i]));
	}
	i = 0;
	FD_ZERO(&all);
	//GETOPT
	if (argc < 2){
		fprintf(stderr, "Must Provide Connection Info\n");
		exit(EXIT_FAILURE);
	}	
	while ((op = getopt(argc, argv, CMD_OP)) != -1){
		i++;
		switch (op){
			case 'p':
				break;
			default:
				printf("Command Line Input Unrecognized!\n");
				exit(EXIT_FAILURE);
		}
	}
	i++;
	pnum = atoi(argv[i]);
	sprintf(pnum_str, "%d", pnum);
	
	listenfd = socket(AF_INET, SOCK_STREAM, 0);

	my_ad.sin_family = AF_INET;
	my_ad.sin_addr.s_addr = htonl(INADDR_ANY); 
	my_ad.sin_port = htons(pnum);
	check = (bind(listenfd, (struct sockaddr*) &my_ad, sizeof(my_ad)));
	if (check != 0){
		perror("Unbound");
		exit(EXIT_FAILURE);
	}
	
	listen(listenfd, LISTENQ);
	printf("Server listening on Port: %s\n", pnum_str);  
	maxfd = listenfd;		
	FD_SET(listenfd, &all);
	
	while(1){
		rset = all;
		fflush(stdout);
		check = select(maxfd+1, &rset, NULL, NULL, NULL);
		printf("Select found %d ready.\n", check);
		if(FD_ISSET(listenfd, &rset)){
			tot_clients++;
			printf("Client Connection Request\n");
			client_len = sizeof(client_ad);
			connfd = accept(listenfd, (struct sockaddr *) &client_ad, &client_len);
			for (i = 0; i < MAX_CLIENTS; i++){
				if (client[i] == -1){
					client[i] = connfd;
					break;
				}
			}
			if (i == MAX_CLIENTS){
				printf("%s - Exceeded Max Clients\n", RETURN_ERROR);
				exit(EXIT_FAILURE);
			}
			FD_SET(connfd, &all);
			if (connfd > maxfd)
				maxfd = connfd;
			if (--check <= 0)
				continue;
		}
		for (i = 0; i < MAX_CLIENTS; i++){
				sockfd = client[i];
				if (sockfd < 0)
					continue;		
			if(FD_ISSET(sockfd, &rset)){
				printf("Received Comm from Client [%d]\n", (i+1));
				memset(cmd, 0, sizeof(cmd));
				check = read(sockfd, cmd, sizeof(cmd));
				if (check == 0){
					close(sockfd);
					FD_CLR(sockfd, &all);
					client[i] = -1;
					printf("Client [%d] Disconnected\n", (i+1));
					tot_clients--;
				}
				else{	//COMMANDS
					printf(":%s:\n", cmd);
					if((strncmp(cmd, CMD_REMOTE_CHDIR, strlen(CMD_REMOTE_CHDIR))) == 0){
						for(k = 0;k < (strlen(cmd)); k++){
							if ( k >= strlen(cmd)- strlen(CMD_REMOTE_CHDIR)){
								cmd[k] = '\0';
								break;
							}
							else
								cmd[k] = cmd[k+strlen(CMD_REMOTE_CHDIR)];
						}
						getcwd(currdir, sizeof(currdir));
						chdir(dir[i]);
						chdir(cmd);
						getcwd(dir[i], sizeof(dir[i]));
						write(sockfd, dir[i], strlen(dir[i]));
						chdir(currdir);
					}
					else if((strncmp(cmd, CMD_REMOTE_HOME, strlen(CMD_REMOTE_HOME))) == 0){
						for(k = 0;k < (strlen(cmd)); k++){
							if ( k >= strlen(cmd)- (strlen(CMD_REMOTE_HOME)+1)){
								cmd[k] = '\0';
								break;
							}
							else
								cmd[k] = cmd[k+(strlen(CMD_REMOTE_HOME)+1)];
						}
						getcwd(currdir, sizeof(currdir));
						chdir(cmd);
						getcwd(dir[i], sizeof(dir[i]));
						write(sockfd, dir[i], strlen(dir[i]));
						chdir(currdir);
					}
					else if((strncmp(cmd, CMD_REMOTE_DIR, strlen(CMD_REMOTE_DIR))) == 0){
						getcwd(currdir, sizeof(currdir));
						chdir(dir[i]);
						pofd = popen(CMD_LS_POPEN, "r");
						if (pofd == NULL){
							perror("POPEN FAIL");
						}
						memset(buff, 0, sizeof(buff));
						while((fgets(buff, sizeof(buff), pofd)) != NULL){
							write(sockfd, buff, sizeof(buff));
						}
						fflush(pofd);
						memset(cmd, 0, sizeof(cmd));
						sprintf(cmd, "%c", EOT_CHAR);
						write(sockfd, cmd, sizeof(cmd));
						pclose(pofd);
						chdir(currdir);
					}
					else if((strncmp(cmd, CMD_REMOTE_PWD, strlen(CMD_REMOTE_PWD))) == 0){
						write(sockfd, dir[i], strlen(dir[i]));
					}
					else if((strncmp(cmd, CMD_PUT_TO_SERVER, strlen(CMD_PUT_TO_SERVER))) == 0){
						int pwfd;
						getcwd(currdir, sizeof(currdir));
						chdir(dir[i]);
						for(k = 0;k < (strlen(cmd)); k++){
							if ( k >= strlen(cmd)- strlen(CMD_PUT_TO_SERVER)){
								cmd[k] = '\0';
								break;
							}
							else
								cmd[k] = cmd[k+strlen(CMD_PUT_TO_SERVER)];
						}
						basename(cmd);
						pwfd = creat(cmd, SEND_FILE_PERMISSIONS);
						memset(buff, 0, sizeof(buff));
						while((read(sockfd, buff, sizeof(buff))) > 0){
							if((buff[0] == EOT_CHAR))								
								break;
							if(strcmp(buff, RETURN_ERROR) == 0){
								remove(cmd);
								break;
							}
							write(pwfd, buff, strlen(buff));
						}
						close(pwfd);
						chdir(currdir);
					}
					else if((strncmp(cmd, CMD_GET_FROM_SERVER, strlen(CMD_GET_FROM_SERVER))) == 0){
						int prfd;
						getcwd(currdir, sizeof(currdir));
						chdir(dir[i]);
						for(k = 0;k < (strlen(cmd)); k++){
							if ( k >= strlen(cmd)- strlen(CMD_PUT_TO_SERVER)){
								cmd[k] = '\0';
								break;
							}
							else
								cmd[k] = cmd[k+strlen(CMD_PUT_TO_SERVER)];
						}
						basename(cmd);
						if ((access(cmd, F_OK)) == -1){
							fprintf(stderr, "File Not Found-> %s", RETURN_ERROR);
							write(sockfd, RETURN_ERROR, sizeof(RETURN_ERROR));
						}
						else{
						prfd = open(cmd, O_RDONLY);
						while((read(prfd, buff, sizeof(buff))) > 0){
							write(sockfd, buff, sizeof(buff));
						}
						memset(buff, 0, sizeof(buff));
						sprintf(buff, "%c", EOT_CHAR);
						write(sockfd, buff, sizeof(buff));
						close(prfd);
						fprintf(stderr,"Sent File-> %s\n", cmd);
						}	
					}
				}
				if(--check <= 0)
					break;
			}	
		}
	}
}
void sig_handler(int num){
    fprintf(stderr, "\nEXIT SIGNAL --> %d\n", num);
    rm_soc();
    exit(num);
}
void rm_soc (void){
int i = 0;
for (; i < MAX_CLIENTS; i++){
		sockfd = client[i];
		if (sockfd < 0)
			continue;
		else
			close(sockfd);
	}
	close(listenfd);
}