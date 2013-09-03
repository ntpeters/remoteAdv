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

char*   server_ip       = "127.0.0.1";           /* Local testing */
// char*    server_ip       = "141.219.153.205";    /* Colossus */
// char*    server_ip       = "141.219.153.206";    /* Guardian */
// char*    server_ip       = "54.214.246.148";     /* DevBox - My Amazon EC2 instance */

int     server_port = 51739;
int     dbgLevel    = 3;
char*   logFile     = "slave.log";
char*   version     = "Development Build";
int     client_type = type_client_slave;

// Function Prototypes
int authenticate( int sockfd );
void death_handler( int signum );
int open_c( int connection, OpHeader command, char* response );
int close_c( int connection, OpHeader command, char* response );
int read_c( int connection, OpHeader command, char* response );
int write_c( int connection, OpHeader command, char* response );
int seek_c( int connection, OpHeader command, char* response );
int exec_c( int connection, OpHeader command, char* response );
int c_kill( int connection, OpHeader command, char* response );

/* Slave Client
   Takes no parameters */
int main(){
    setDebugLevel( dbgLevel );
    setLogFile( logFile );

    // Ignore broken pipe signal when server disconnects
    signal( SIGPIPE, death_handler );

    writeLog( 0, "Starting remoteAdv Slave Client - Version: %s", version );
    
    // If the client is unexpectedly disconnected it will try to reconnect 3 times
    for( int i = 0; i <= 3; i++ ) {

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
            //perror("ERROR: Connection to server failed");
            writeLog( -1, "Connection to server failed. Retrying..." );

            fflush( stdout );
            sleep( 3 );
            continue;
        }

        int auth_result = authenticate( sockfd );
        if( auth_result == -1 ) {
            writeLog( -1, "Server authentication failed" );
            exit(1);
        } else {
            writeLog( 2, "Server connection successful" );
            writeLog( 3, "Connected to server at '%s:%d'", server_ip, server_port );
        }

        OpHeader command;
        int read_result;

        // Wait to recieve commands
        while( ( read_result = read( sockfd, &command, sizeof( command ) ) ) ) {
            int respSize = 0;
            char* response = NULL;
            
            int opcode = command.opcode;
            writeLog( 3, "Opcode Recieved: %d", opcode );

            switch( opcode ) {
                case open_call: 
                    respSize = open_c( sockfd, command, response );
                    break;
                case close_call:
                    respSize = close_c( sockfd, command, response );
                    break;
                case read_call:
                    respSize = read_c( sockfd, command, response );
                    break;
                case write_call:
                    respSize = write_c( sockfd, command, response );
                    break;
                case seek_call:
                    respSize = seek_c( sockfd, command, response );
                    break;
                case exec_call:
                    respSize = exec_c( sockfd, command, response );
                    break;
                case c_master_kill_client:
                    respSize = c_kill( sockfd, command, response );
                default:
                    writeLog( -1, "Invalid OpCode: %d", opcode );
                    break;
            }
        }

        // Close the connection to the server
        close(sockfd);

        // If a bad read occured, we have been disconnected
        if( read_result <= 0 ) {
            death_handler(0);
        }
    }

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
    Handles unexpected disconnects from the server.
    Only logs messages and then returns flow back to the main program.

    Input:
    int signum - The signal this handler is handling. '0' if no signal.
*/
void death_handler( int signum ) {
    if( signum == SIGPIPE ) {
        writeLog( 1, "Pipe broken signal received" );
    }

    writeLog( 1, "Server unexpectedly disconnected! Trying to reconnect..." );
}

/*
     Handles a remote open call.

     Input:
     int connection - Socket connection to read from.
     char* response - The constructed response to send back to the client.

     Return Value:
     The size of the response, or -1 if something fails.
*/
int open_c( int connection, OpHeader command, char* response ) {
     writeLog( 3, "Open call recieved from client" );

     int respSize = 0;

     // TODO: Implement this function.

     return respSize;
}

/*
     Handles a remote close call.

     Input:
     int connection - Socket connection to read from.
     char* response - The constructed response to send back to the client.

     Return Value:
     The size of the response, or -1 if something fails.
*/
int close_c( int connection, OpHeader command, char* response ) {
     writeLog( 3, "Close call recieved from client" );

     int respSize = 0;

     // TODO: Implement this function.

     return respSize;
}

/*
     Handles a remote read call.

     Input:
     int connection - Socket connection to read from.
     char* response - The constructed response to send back to the client.

     Return Value:
     The size of the response, or -1 if something fails.
*/
int read_c( int connection, OpHeader command, char* response ) {
     writeLog( 3, "Read call recieved from client" );

     int respSize = 0;

     // TODO: Implement this function.

     return respSize;
}

/*
     Handles a remote write call.

     Input:
     int connection - Socket connection to read from.
     char* response - The constructed response to send back to the client.

     Return Value:
     The size of the response, or -1 if something fails.
*/
int write_c( int connection, OpHeader command, char* response ) {
     writeLog( 3, "Write call recieved from client" );

     int respSize = 0;

     // TODO: Implement this function.

     return respSize;
}

/*
     Handles a remote seek call.

     Input:
     int connection - Socket connection to read from.
     char* response - The constructed response to send back to the client.

     Return Value:
     The size of the response, or -1 if something fails.
*/
int seek_c( int connection, OpHeader command, char* response ) {
     writeLog( 3, "Seek call recieved from client" );

     int respSize = 0;

     // TODO: Implement this function.

     return respSize;
}

/*
     Handles a remote exec call.

     Input:
     int connection - Socket connection to read from.
     char* response - The constructed response to send back to the client.

     Return Value:
     The size of the response, or -1 if something fails.
*/
int exec_c( int connection, OpHeader command, char* response ) {
     writeLog( 3, "Exec call recieved from client" );

     int respSize = 0;

     // TODO: Implement this function.

     return respSize;
}

int c_kill( int connection, OpHeader command, char* response ) {
    writeLog( 0, "Kill command recieved. Exiting client now." );
    exit( 0 );
}