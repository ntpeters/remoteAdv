#include "opCodes.h"
#include "sockaddrAL.h"
#include "util.h"
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

/* 141.219.153.205 for wopr     */
/* 141.219.153.206 for guardian */

// Global Variables
/*
     User defined TODO: set by passed arg
     Debug Levels:
     0 = none
     1 = warnings only (default)
     2 = basic debug
     3 = verbose debug
*/
int dbgLevel = 3;
int server_portnumber = 51739;     // Port must be constant due to the nature of this project
char* logFile = "server.log";

// Function Prototypes
int open_c( int connection, char* response );
int close_c( int connection, char* response );
int read_c( int connection, char* response );
int write_c( int connection, char* response );
int seek_c( int connection, char* response );
int exec_c( int connection, char* response );

// Output is standard out
int main( int argc , char* argv[] ){
     setDebugLevel( dbgLevel );
     setLogFile( logFile );
     // Logger tests
     // writeLog( -2, "Test fatal" );
     // writeLog( -1, "Test error" );
     // writeLog( 0, "Test info" );
     // writeLog( 1, "Test warning" );
     // writeLog( 2, "Test debug" );
     // writeLog( 3, "Test debug-verbose" );
     
     
     int listenFD;
     int connectFD;
     socklen_t length;
     struct sockaddr_in s1;
     struct sockaddr_in s2;
     
     /* ---- Initialize port and print ---- */
     
     listenFD = socket( AF_INET , SOCK_STREAM , 0 );
     if( listenFD == -1 ) {
          writeLog( -2, "Open socket failed" );
          exit(1);
     }
     
     bzero( (char*) &s1 , sizeof( s1 ) );                           /* Zero out struct */
     s1.sin_family = AF_INET;
     s1.sin_addr.s_addr = INADDR_ANY;
     s1.sin_port = server_portnumber;                               /* Assign a port number */
     
     int bindResult = bind( listenFD , (struct sockaddr*) &s1 , sizeof( s1 ) );        /* Bind an availible port to our fd */
     if( bindResult == -1 ) {
          writeLog( -2, "Bind failed" );
          exit(1);
     }

     length = sizeof( s1 );
     getsockname( listenFD ,  (struct sockaddr*) &s1 , &length );

     writeLog( 0, "Listening on port: %d", s1.sin_port );           /* Print the port number */

     signal( SIGCHLD , SIG_IGN );                                   /* So we don't wait on zombies */
     int listenResult = listen( listenFD , max_connections );       /* I don't think we'll hit 512 clients.. */
     if( listenResult == -1 ) {
          writeLog( -2, "Unable to listen" );
          exit(1);
     }

     /* ---- Keep listening for clients ---- */
     while(1) {

          length  = sizeof( s2 );
          connectFD = accept( listenFD , (struct sockaddr*) &s2 , &length );    /* Accept connection */
          if( connectFD == -1 ) {
               writeLog( -1, "Connection to client failed" );
          } else {
               int client_type = 0;
               read( connectFD, &client_type, sizeof( client_type ) );
               int resp = 1;
               write( connectFD, &resp, sizeof( resp ) );
               addInfo( s2, client_type );
               char* message = (char*)malloc(5);
               message = getClientList( message );
               writeLog( 2, "Client List:%s", message );
               free( message );
          }
          
          if( !fork() ){                                                        /* Create child      */
               close( listenFD );                                               /* Close one end of the pipe */

               writeLog( 2, "Client connection successful");

               int opcode = 0;
               int respSize = 0;
               char* response = NULL;

               OpHeader opH;
                    
               int isDone = 1;
               while( ( isDone = read( connectFD , &opH , sizeof( opcode ) ) ) ) {   // Get the opcode
                    opcode = opH.opcode;
                    writeLog( 3, "Opcode Recieved: %d", opcode );

                    switch( opcode ) {
                         case open_call: 
                              respSize = open_c( connectFD, response );
                              break;
                         case close_call:
                              respSize = close_c( connectFD, response );
                              break;
                         case read_call:
                              respSize = read_c( connectFD, response );
                              break;
                         case write_call:
                              respSize = write_c( connectFD, response );
                              break;
                         case seek_call:
                              respSize = seek_c( connectFD, response );
                              break;
                         case exec_call:
                              respSize = exec_c( connectFD, response );
                              break;
                         default:
                              writeLog( -1, "Invalid OpCode: %d", opcode );
                              break;
                    }

                    // TODO: send response to client

                    free( response );
               } // End of while client actions loop
               
               if( isDone == -1 ) {
                    writeLog( 2, "Client connection has been lost");
               }
               
               close( connectFD );
               deleteInfo( s2 );
               writeLog( 3, "Child process is done. PID: %d", getpid() );
               exit( 0 );           /* Child exits when done */
          } else {
               close( connectFD );
               deleteInfo( s2 );
          }
     } // end of while

     return 0;
}

/*
     Handles a remote open call.

     Input:
     int connection - Socket connection to read from.
     char* response - The constructed response to send back to the client.

     Return Value:
     The size of the response, or -1 if something fails.
*/
int open_c( int connection, char* response ) {
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
int close_c( int connection, char* response ) {
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
int read_c( int connection, char* response ) {
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
int write_c( int connection, char* response ) {
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
int seek_c( int connection, char* response ) {
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
int exec_c( int connection, char* response ) {
     writeLog( 3, "Exec call recieved from client" );

     int respSize = 0;

     // TODO: Implement this function.

     return respSize;
}