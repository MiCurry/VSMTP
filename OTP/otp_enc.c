/*
 * otp_enc.c
 *
 *  Created on: Mar 1, 2015
 *      Author: coanj
 *
 * 	This program connects to otp_enc_d, and asks it to perform a one-time pad style
 * 	encryption. By itself, otp_enc doesn’t do the encryption.
 *
 * 	Its syntax is as follows:
 * 	otp_enc plaintext key port
 */ 

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <ctype.h>

void error( const char *msg ) {
	fprintf(stderr, "%s\n", msg);
	exit( 1 );
}

int main( int argc, char *argv[ ] ) {
	int sockfd, portnumber, n;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	
	//	if command line is used incorrectly
	if( argc<4 ) error( "Usage: otp_enc plaintext key port\n");

	FILE *fp;
	
	//	Get local directory
	char dir[512];
	getcwd(dir,sizeof(dir));
	
	//	Get name and location of text file
	char textLoc[512]="";
	strcat(textLoc, dir);
	strcat(textLoc,"/");
	strcat(textLoc,argv[ 1 ]);
	
	//	Get name and location of key file
	char keyLoc[512]="";
	strcat(keyLoc,dir);
	strcat(keyLoc,"/");
	strcat(keyLoc,argv[ 2 ]);
	
	//	Count chars in text file
	fp = fopen(textLoc, "r");
	if(fp == NULL)
		error("Error, text file not found.\n");
	int ch, charCount = 0;
	while(1){
		ch = fgetc(fp);
		if( ch == '\n' || ch == EOF ){
			break;
		}
		charCount++;
	}
	fclose(fp);
	
	//	create array based on char count then
	//	store text into textfile array
	char textFile[charCount];
	fp = fopen(textLoc, "r");
	int index = 0;
	while(index != charCount){
		ch = fgetc(fp);
		//	Make sure characters are alphabetic and proper
		if(!isalpha(ch)){
			if(ch!=' '){
				fprintf(stderr,"Error: %s contains bad characters\n",argv[ 1 ]);
				exit( 1 );
			}
		}
		textFile[index++]=ch;
	}
	fclose(fp);
	
	//	Count chars in key file
	fp = fopen(keyLoc, "r");
	if(fp == NULL)
		error("Error, key file not found.\n");
	int keyCount = 0;
	ch = 0;
	while(1){
		ch = fgetc(fp);
		if( ch == '\n' || ch == EOF ){
			break;
		}
		keyCount++;
	}
	fclose(fp);
	//	create array based on char count then
	//	store key into key array
	char keyFile[keyCount];
	fp = fopen(keyLoc, "r");
	index = 0;
	while(index != sizeof(textFile)){
		ch = fgetc(fp);
		if( ch == '\n' || ch == EOF ){
			fprintf(stderr,"Error: key file '%s' is too short\n",argv[ 2 ]);
			exit( 1 );
		}
		keyFile[index++]=ch;
	}
	fclose(fp);


	//	Set up sockets for reading a writing
	portnumber = atoi( argv[ 3 ] );
	sockfd = socket( AF_INET, SOCK_STREAM, 0 );
	if( sockfd<0 ) error( "Error opening socket\n" );
	//	since this is only local, still need to error check though
	server = gethostbyname( "localhost" );
	if( server==NULL ) error( "Error, no such host\n" );

	//	Set all values in buffer to zero
	memset( ( char * ) &serv_addr, '\0', sizeof( serv_addr ) );
	serv_addr.sin_family = AF_INET;
	bcopy( ( char * ) server->h_addr, ( char * ) &serv_addr.sin_addr.s_addr, server->h_length );
	//	convert address to network byte order
	serv_addr.sin_port = htons( portnumber );
	//	On error connecting
	if( connect( sockfd, ( struct sockaddr * ) &serv_addr, sizeof( serv_addr ) )<0 ){
		fprintf(stderr,"Error: could not contact otp_enc_d on port %d\n",portnumber);
		exit( 2 );
	}
	
	
	//	************************************************************************************
	
	
	//	Send size of text that will be encoded
	n = write( sockfd, &charCount, sizeof(charCount) );
	if( n<0 ) error( "Error writing to socket\n" );
	
	//	send the text to be encoded
	textFile[charCount]='\0';
	n = write( sockfd, &textFile, sizeof(textFile) );
	if( n<0 ) error( "Error writing to socket\n" );
	
	//	send the key used to encode
	keyFile[charCount]='\0';
	n = write( sockfd, &keyFile, sizeof(keyFile) );
	if( n<0 ) error( "Error writing to socket\n" );
	
	//	read what the daemon sends back
	memset( textFile, '\0', charCount );
	n = read( sockfd, textFile, sizeof(textFile) );
	if( n<0 ) error( "Error reading from socket\n" );
	
	//	output result
	printf( "%s\n", textFile );
	
	//	************************************************************************************
	
	close( sockfd );
	sleep(1);
	return 0;
}