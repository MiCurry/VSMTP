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
#define DEBUG
#define CMD_OP "pi"
//GLOBALS
static int sockfd;
static FILE* pofd;
//STRUCTS
//
//
//
//PROTOTYPES
void sig_handler(int);
void rm_soc(void);
//MAIN
int main (int argc, char* argv[], char* envp[]){
//DECLARATIONS
	int op = 0;
	int i = 0;
	int check;

	char* input = NULL;
	int pnum;
	char cmd[MAXLINE];
	char buff[MAXLINE];
	char buff2[MAXLINE];
	char currdir[MAXLINE];
	char pnum_str[12];
	char ip_ad[50];
	
	struct sockaddr_in servaddr;
//MEMORY MANAGEMENT
//GETOPT
	if (argc < 2){
		fprintf(stderr, "Must Provide Connection Info\n");
		exit(EXIT_FAILURE);
	}
	while ((op = getopt(argc, argv, CMD_OP)) != -1){
		i++;
		switch (op){
			case 'p':
				pnum = atoi(argv[i+1]);
				sprintf(pnum_str, "%d", pnum);
				break;
			case 'i':
				strcpy(ip_ad, argv[i+1]);
				break;
			default:
				printf("Command Line Input Unrecognized!\n");
				exit(EXIT_FAILURE);
		}
		i++;
	}
	i++;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	memset(&servaddr, 0, sizeof(servaddr));	
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(pnum);
	inet_pton(AF_INET, ip_ad, &servaddr.sin_addr.s_addr);

	check = connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr));
    if (check != 0){
		perror("Connection Failed");
		exit(EXIT_FAILURE);
	}		
	atexit(rm_soc);	
//COMMUNICATE TO SERVER
//COMMAND PROMPT
	while(1){
		memset(cmd, 0, sizeof(cmd));
		memset(buff, 0, sizeof(buff));
		memset(buff2, 0, sizeof(buff2));
		
		fputs(PROMPT, stdout);
		fflush(stdout);	
		input = fgets(cmd, sizeof(cmd), stdin);
		if (input == NULL)
			break;
		if((input = strchr(cmd, '\n')) != NULL){
			*input = '\0';
		}	
		if ((strncmp(cmd, CMD_HELP, strlen(CMD_HELP))) == 0){
		fprintf(stderr,"Listing of Client Commands:\n");
		fprintf(stderr,"\thelp\t\t: You are Here!\n");
		fprintf(stderr,"\texit\t\t: Exit Client\n");
		fprintf(stderr,"\tcd <dir>\t: Change to directory <dir> on Server\n");
		fprintf(stderr,"\tlcd <dir>\t: Change to directory <dir> on Client\n");
		fprintf(stderr,"\tdir\t\t: Display Directory Contents of Server\n");
		fprintf(stderr,"\tldir\t\t: Display Directory Contents of Client\n");
		fprintf(stderr,"\thome\t\t: Change current Client directory to user home directory\n");
		fprintf(stderr,"\tlhome\t\t: Change current Server directory to user home directory\n");
		fprintf(stderr,"\tpwd\t\t: Display current Server directory\n");
		fprintf(stderr,"\tlpwd\t\t: Display current Client directory\n");
		fprintf(stderr,"\tget <file>\t: Transfer <file> from Server to Client\n");
		fprintf(stderr,"\tput <file>\t: Transfer <file> from Client to Server\n");
		fprintf(stderr,"\nYou are connected with IP: %s\n", ip_ad);
		fprintf(stderr,"You are using Port: %d\n", pnum);
		}
        else if ((strncmp(cmd, CMD_EXIT, strlen(CMD_EXIT))) == 0){
        //EXIT PROGRAM
			memset(buff, 0, sizeof(buff));
            write(sockfd, buff, strlen(buff));
			close(sockfd);
			break;
        }
		else if((strncmp(cmd, CMD_LOCAL_CHDIR, strlen(CMD_LOCAL_CHDIR))) == 0){
			for(i = 0;i < (strlen(cmd)); i++){
				if ( i >= strlen(cmd)- (strlen(CMD_LOCAL_CHDIR))){
					cmd[i] = '\0';
					break;
				}
				else
					cmd[i] = cmd[i+(strlen(CMD_LOCAL_CHDIR))];
			}
			check = chdir(cmd);
			if (check == -1)
				perror("lcd Failed");
			getcwd(currdir, sizeof(currdir));
			currdir[FILENAME_MAX - 1] = '\0';
			fprintf(stderr, "Current Client Directory:\n\t%s\n", currdir);
		}
		else if((strncmp(cmd, CMD_LOCAL_DIR, strlen(CMD_LOCAL_DIR))) == 0){
			pofd = popen(CMD_LS_POPEN, "r");
			if (pofd == NULL){
				perror("POPEN FAIL");
			}
			while((fgets(buff2, sizeof(buff2), pofd)) != NULL)
				fprintf(stderr, "%s", buff2);
			pclose(pofd);
		}
		else if((strncmp(cmd, CMD_LOCAL_HOME, strlen(CMD_LOCAL_HOME))) == 0){
			chdir(getenv("HOME"));
			getcwd(currdir, sizeof(currdir));
			currdir[FILENAME_MAX - 1] = '\0';
			fprintf(stderr, "Current Client Directory:\n\t%s\n", currdir);
		}
		else if((strncmp(cmd, CMD_LOCAL_PWD, strlen(CMD_LOCAL_PWD))) == 0){
			getcwd(currdir, sizeof(currdir));
			currdir[sizeof(currdir) - 1] = '\0';
			fprintf(stderr, "Current Client Directory:\n\t%s\n", currdir);
		}
		else if((strncmp(cmd, CMD_REMOTE_CHDIR, strlen(CMD_REMOTE_CHDIR))) == 0){
			write(sockfd, cmd, strlen(cmd));
			check = read(sockfd, buff2, sizeof(buff2));
			fprintf(stderr, "Current Client_Server Directory:\n\t%s\n", buff2);
		}
		else if((strncmp(cmd, CMD_REMOTE_HOME, strlen(CMD_REMOTE_HOME))) == 0){
			input = getenv("HOME");
			sprintf(buff2, " %s", input);
			strcat(cmd, buff2);
			write(sockfd, cmd, strlen(cmd));
			memset(buff2, 0, sizeof(buff2));
			check = read(sockfd, buff2, sizeof(buff2));
			fprintf(stderr, "Current Client_Server Directory:\n\t%s\n", buff2);
		}
		else if((strncmp(cmd, CMD_REMOTE_DIR, strlen(CMD_REMOTE_DIR))) == 0){
			char s = EOT_CHAR;
			write(sockfd, cmd, strlen(cmd));
			//while((fgets(buff, sizeof(buff), sp))!= NULL){
			while((check = read(sockfd, buff, sizeof(buff))) > 0){
				if (buff[0] == s){
					break;
				}
				printf("%s", buff);
			}
		}
		else if((strncmp(cmd, CMD_REMOTE_PWD, strlen(CMD_REMOTE_PWD))) == 0){
			write(sockfd, cmd, strlen(cmd));
			check = read(sockfd, buff2, sizeof(buff2));
			fprintf(stderr, "Current Client_Server Directory:\n\t%s\n", buff2);			
		}
		else if((strncmp(cmd, CMD_PUT_TO_SERVER, strlen(CMD_PUT_TO_SERVER))) == 0){
			int prfd;
			int k;
			write(sockfd, cmd, strlen(cmd));
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
				while((read(prfd, buff2, sizeof(buff2))) > 0){
					write(sockfd, buff2, sizeof(buff2));
				}
				memset(buff, 0, sizeof(buff));
				sprintf(buff, "%c", EOT_CHAR);
				write(sockfd, buff, sizeof(buff));
				close(prfd);
				fprintf(stderr,"Sent File-> %s\n", cmd);				
			}
		}
		else if((strncmp(cmd, CMD_GET_FROM_SERVER, strlen(CMD_GET_FROM_SERVER))) == 0){
			int pwfd;
			int k;
			write(sockfd, cmd, strlen(cmd));
			for(k = 0;k < (strlen(cmd)); k++){
							if ( k >= strlen(cmd)- strlen(CMD_GET_FROM_SERVER)){
								cmd[k] = '\0';
								break;
							}
							else
								cmd[k] = cmd[k+strlen(CMD_GET_FROM_SERVER)];
			}
			basename(cmd);
			pwfd = creat(cmd, SEND_FILE_PERMISSIONS);
				while((read(sockfd, buff2, sizeof(buff2))) > 0){
							if((buff2[0] == EOT_CHAR))								
								break;
							if(strcmp(buff2, RETURN_ERROR) == 0){
								remove(cmd);
								break;
							}					
					write(pwfd, buff2, strlen(buff2));
				}
			close(pwfd);
		}
        else
            printf("%s --> CMD_UNKNOWN\n", RETURN_ERROR);	
	}
    return(EXIT_SUCCESS);
}
void sig_handler(int num){
    fprintf(stderr, "\nEXIT SIGNAL --> %d\n", num);
    //CLEANUP HERE
	close(sockfd);
    exit(num);
}
void rm_soc (void){
	close(sockfd);
}