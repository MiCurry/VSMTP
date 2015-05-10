/*
 * keygen.c
 *
 *  Created on: Mar 1, 2015
 *      Author: coanj
 */


/*
 * 	This program creates a key file of specified length. The characters
 * 	in the file generated will be any of the 27 allowed characters.
 *
 * 	The syntax for keygen is as follows:
 * 	keygen keylength
 *
 * 	Where keylength is the length of the key file in characters.
 * 	Here is an example run, which stores a key file of 256 characters in a file called “mykey”:
 * 	% keygen 256 > mykey
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[]) {

	int i;
	srand(time(NULL));

	//	On error
    if(argc<=1) {
    	error("Error: missing keylength!\nUsage: keygen keylength\n");
    }
    
    //	Generates random alphabetical characters or space
    for(i=0; i<atoi(argv[1]); i++){
		char randomletter = "ABCDEFGHIJKLMNOPQRSTUVWXYZ "[random () % 27];
		putc(randomletter, stdout);
    }
    return 0;
 }