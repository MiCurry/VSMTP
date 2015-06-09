/*
 * otp_dec_d.c
 *
 *  Created on: Mar 1, 2015
 *      Author: coanj
 *
 * 	This program will run in the background as a daemon. Its function is to perform the actual
 * 	decoding. This program will listen on a particular port, assigned when it is first ran, and
 * 	receives encrypted text and a key via that port when a connection to it is made. It will then write
 * 	back the decoded text to the process that it is connected to via the same port.
 *
 * 	Use this syntax for otp_dec_d:
 * 	otp_dec_d port &
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

void error( const char *msg ) {
	fprintf(stderr, "%s\n", msg);
	exit( 1 );
}

int main( int argc, char* argv[ ] ) {
	pid_t process_id = 0;
	pid_t sid = 0;

	//	Ensure port is added
	if( argc<2 ) error( "Error, no port provided\n" );

	// Create child process
	process_id = fork( );

	// is fork successful or not, if so close parent
	if( process_id<0 )
		error( "Error, unable to fork process!\n" );
	else if( process_id>0 ) {
		exit( 0 );
	}

	//	Create daemon
	sid = setsid( );
	if( sid<0 ) error( "Error, failed to create daemon process!\n" );
	
	//	Daemon process code
	while( 1 ) {

		//	Get socket set up for reading input
		int sockfd, newsockfd, portno, n;
		socklen_t clientLength;

		//	Setup socket for input/output
		struct sockaddr_in serv_addr, clientAddress;
		sockfd = socket( AF_INET, SOCK_STREAM, 0 );
		if( sockfd<0 ) error( "Error opening socket" );
		memset( ( char * ) &serv_addr, '\0', sizeof( serv_addr ) );
		portno = atoi( argv[ 1 ] );
		serv_addr.sin_family = AF_INET;
		serv_addr.sin_addr.s_addr = INADDR_ANY;
		serv_addr.sin_port = htons( portno );
		if( bind( sockfd, ( struct sockaddr * ) &serv_addr, sizeof( serv_addr ) )<0 )
		    error( "Error on binding" );
		listen( sockfd, 5 );
		clientLength = sizeof( clientAddress );
		newsockfd = accept( sockfd, ( struct sockaddr * ) &clientAddress, &clientLength );
		if( newsockfd<0 ) error( "Error on accept" );

		//	************************************************************************************
		
		//	Get size of textFile
		int size = 0;
		n = read( newsockfd, &size, sizeof(int) );
		if( n<0 ) error( "Error reading from socket" );
		
		//	Get textFile
		char textFile[ size ];
		memset( textFile, '\0', sizeof(textFile) );
		n = read( newsockfd, &textFile, sizeof(textFile) );
		if( n<0 ) error( "Error reading from socket" );
		
		//	Get keyFile
		char keyFile[ size ];
		memset( keyFile, '\0', sizeof(keyFile) );
		n = read( newsockfd, &keyFile, sizeof(keyFile) );
		if( n<0 ) error( "Error reading from socket" );
		
		//	Decryption
		char value[] = { "ABCDEFGHIJKLMNOPQRSTUVWXYZ " }; // used for conversion
		int index = 0;
		while(size--){
			int textConvert, keyConvert;
			char cipher;
			
			//	convert each char and space to use in value array
			if(textFile[index]==32){
				textConvert = 26;
			} else {
				textConvert = (textFile[index]-65);
			}
			
			if(keyFile[index]==32){
				keyConvert = 26;
			} else {
				keyConvert = (keyFile[index]-65);
			}
			
			//	decode the one-time pad method
			cipher = ( textConvert - keyConvert ) % 27;
			if(cipher<0) cipher += 27;
			
			//	new decrypted textFile
			textFile[index]=value[cipher];
			index++;	
		}
		
		//	Write
		n = write( newsockfd, &textFile, sizeof(textFile) );
		if( n<0 ) error( "Error writing to socket" );
		
		//	************************************************************************************
		
		close( newsockfd );
		close( sockfd );
	}

	return ( 0 );
}