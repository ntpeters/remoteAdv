#include "opCodes.h"
#include "util.h"
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
#include <signal.h>

#define sel_list_slaves		1
#define sel_claim_client	2
#define sel_release_client	3
#define sel_set_debug_level	4
#define sel_kill_client		5
#define sel_close 			6

/* Slave Client
   Takes no parameters */

char*	server_ip		= "127.0.0.1";		/* Just for testing. Change this later*/
											/* 141.219.153.205 for wopr     */
											/* 141.219.153.206 for guardian */
											/* 54.214.246.148 for my Amazon EC2 instance */
int 	server_port		= 51739;
int 	dbgLevel 		= 3;
char* 	logFile 		= "master.log";
char*	version 		= "Development Build";
int 	client_type		= type_client_master;
int 	claimed_client 	= -1;

// Function Prototypes
int authenticate( int sockfd );
void printMenu();
void list_slaves( int sockfd );
void claim_client( int sockfd );
void release_client( int sockfd );
void set_debug_level( int sockfd );
void kill_client( int sockfd );
   
int main(){
	setDebugLevel( dbgLevel );
	setLogFile( logFile );
	setSilentMode(1);

	writeLog( 0, "Starting remoteAdv Master Client - Version: %s", version );
	
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

    int auth_result = authenticate( sockfd );
    if( auth_result == -1 ) {
    	writeLog( -1, "Server authentication failed" );
    	exit(1);
    } else {
    	writeLog( 2, "Server connection successful" );
    	writeLog( 3, "Connected to server at '%s:%d'", server_ip, server_port );
    }

    int end = 0;

    // Wait to send commands
    while( !end ) {
    	printMenu();

    	int input = 0;
    	scanf( "%d", &input );

    	switch( input ) {
    		case sel_list_slaves:
    			system( "clear" );
    			list_slaves( sockfd );
    			break;
    		case sel_claim_client:
    			system( "clear" );
    			claim_client( sockfd );
    			break;
    		case sel_release_client:
    			release_client( sockfd );
    			break;
    		case sel_set_debug_level:
    			set_debug_level( sockfd );
    			break;
    		case sel_kill_client:
    			kill_client( sockfd );
    			break;
    		case sel_close:
    			end = 1;
    		default:
    			break;
    	}

    	fflush( stdout );
    	system( "clear" );
    }

	// Close the connection to the server
	close( sockfd );

    return 0;
}

/*
	Basic authentication when connecting to the server.
	This sends the client type to the server, and expects
	a certain response from the server.

	Input:
	int sockfd - The socket connection to the server

	Return:
	0  - successful connection
	-1 - connection failed
*/
int authenticate( int sockfd ) {
	int response = 0;

	// Send client type to server
    write( sockfd, &client_type, sizeof( client_type ) );
    // Recieve response from server
    read( sockfd, &response, sizeof( response ) );

    if( response == type_server ) {
    	return 0;
    } else { 
    	return -1;
    }
}

/*
	Prints the main menu
*/
void printMenu() {
	printf( "remoteAdv Master Client - Version: %s\n", version );
	printf( "-------------------\n");
	printf( "Select an action\n" );
	printf( "-------------------\n");
	printf( "1 - List Slaves\n" );
	printf( "2 - Claim Client\n" );
	printf( "3 - Release Client\n" );
	printf( "4 - Set Debug Level\n" );
	printf( "5 - Kill Client\n" );
	printf( "6 - Exit\n");
	printf( "-------------------\n");
}

/*
	Lists the slave clients currently connected to the server

	Input:
	int sockfd - The socket connection to the server
*/
void list_slaves( int sockfd ) {

	int request_type = command_sent;
	int request = c_list_slaves;

    write( sockfd, &request_type, sizeof( int ) );
	write( sockfd, &request, sizeof( request ) );

	char* response = (char*)malloc(1000);
	memset( response, 0, 1000 );
	read( sockfd, response, 1000 );

	printf( "Active Slaves" );

	printf( "%s", response );
	printf( "--------------------------------------------------------\n" );

	free( response );

	printf( "\nPress 'Enter' to return to the menu..." );
	
	getchar();
	getchar();
}

/*
	Exclusively claims a slave client for this master client.
	This prevents other masters from sending commands to that client.

	Input:
	int sockfd - The socket connection to the server
*/
void claim_client( int sockfd ) {
	int request_type = command_sent;
	int request = c_list_slaves;

    write( sockfd, &request_type, sizeof( int ) );
	write( sockfd, &request, sizeof( request ) );

	char* response = (char*)malloc(1000);
	memset( response, 0, 1000 );
	read( sockfd, response, 1000 );

	printf( "Select the client to claim" );

	printf( "%s", response );
	printf( "--------------------------------------------------------\n" );

	free( response );

	int choice = -1;
	getchar();
	printf( "Client to claim: " );
	scanf( "%d", &choice );

	request = c_claim;
	write( sockfd, &request_type, sizeof( int ) );
	write( sockfd, &request, sizeof( request ) );

	write( sockfd, &choice, sizeof( choice ) );

	int claim_result = 0;
	read( sockfd, &claim_result, sizeof( int ) );

	if( claim_result == -1 ) {
		writeLog( 3, "Claim Failed: Client already claimed." );
		printf( "Claim Failed: Client already claimed.\n" );
		printf( "\nPress 'Enter' to return to the menu..." );

		getchar();
		getchar();
	} else {
		claimed_client = choice;
	}
}

/*
	Releases the currently claimed client

	Input:
	int sockfd - The socket connection to the server
*/
void release_client( int sockfd ) {
	if( claimed_client == -1 ) {
		printf( "Can't Release: No client claimed\n" );
		printf( "\nPress 'Enter' to return to the menu..." );

		getchar();
		getchar();
	} else {
		printf( "Are you sure you want to release the currenlty claimed client? (y/n) " );
		char confirm;
		getchar();
		scanf( "%c", &confirm );

		if( confirm == 'y' ) {
			int request_type = command_sent;
			int request = c_release;

		    write( sockfd, &request_type, sizeof( int ) );
			write( sockfd, &request, sizeof( request ) );

			write( sockfd, &claimed_client, sizeof( claimed_client ) );
		}
	}
}

/*
	Sets the debug level of the currently claimed client

	Input:
	int sockfd - The socket connection to the server
*/
void set_debug_level( int sockfd ) {

}

/*
	Kills the currently claimed client

	Input:
	int sockfd - The socket connection to the server
*/
void kill_client( int sockfd ) {

}