#include "opCodes.h"
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <strings.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

/* Slave Client
   Takes no parameters */

char*	server_ip	= "127.0.0.1";		/* Just for testing. Change this later*/
int 	server_port	= 51739;
   
int main(){
	
	// Gets a hostent struct containing data about the given host
	struct hostent *server = gethostbyname( server_ip );
	if(server == NULL) {
		herror("ERROR: Host lookup failed");
		exit(1);
	}

	// Create a TCP socket and return a file descriptor for accessing it
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd == -1) {
		perror("ERROR: Open socket failed");
		exit(1);
	}

	// Create and initialize a sock_addr struct contianing info about the server to connect to
	struct sockaddr_in server_addr;
	bzero((char *) &server_addr, sizeof(server_addr));
	// Denotes an internet socket
    server_addr.sin_family = AF_INET;
    // Copies several values from the server hostent struct into this one
    bcopy((char*)server->h_addr, (char*)&server_addr.sin_addr.s_addr, server->h_length);
    // Sets the server port to the one passed in
    server_addr.sin_port = server_port;

    // Connect to the server
    int connectResult = connect(sockfd,(struct sockaddr*)&server_addr, sizeof(server_addr));
    if(connectResult == -1) {
    	perror("ERROR: Connection to server failed");
    	exit(1);
    }

    int op = 1;
    char* msg = (char*)malloc(sizeof(op));
    memcpy(msg, &op, sizeof(op));
    write(sockfd, msg, sizeof( msg ));

	// Close the connection to the server
	close(sockfd);

    return 0;
}