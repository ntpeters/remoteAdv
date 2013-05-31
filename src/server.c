#include "opCodes.h"
#include "sockaddrAL.h"
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
#include <time.h>


/* 141.219.153.205 for wopr     */
/* 141.219.153.206 for guardian */

// Global Variables
int dbgLevel = 2;                  // Defined by user: 0 = none, 1 = basic , 2 = max TODO: Will have to change by signal. 
int server_portnumber = 51739;     // Port must be constant due to the nature of this project
char* logFile = "server.log";

// Function Prototypes
void writeLog( int loglvl , char* str );
char* getDateString();

// Output is standard out
int main( int argc , char* argv[] ){
     
     int listenFD;
     int connectFD;
     socklen_t length;
     struct sockaddr_in s1;
     struct sockaddr_in s2;
     
     /* ---- Initialize port and print ---- */
     
     listenFD = socket( AF_INET , SOCK_STREAM , 0 );
     
     bzero( (char*) &s1 , sizeof( s1 ) );                           /* Zero out struct */
     s1.sin_family = AF_INET;
     s1.sin_addr.s_addr = INADDR_ANY;
     s1.sin_port = server_portnumber;                               /* Assign a port number */
     
     bind( listenFD , (struct sockaddr*) &s1 , sizeof(s1) );        /* Bind an availible port to our fd */
     length = sizeof( s1 );
     getsockname( listenFD ,  (struct sockaddr*) &s1 , &length );
     printf( "%d\n", s1.sin_port );                                 /* Print the port number */
     signal( SIGCHLD , SIG_IGN );                                   /* So we don't wait on zombies */
     listen( listenFD , 512 );                                      /* I don't think we'll hit 512 clients.. */

     /* ---- Keep listening for clients ---- */
     int isDone = 1;
     while(1){

          length  = sizeof( s2 );
          connectFD = accept( listenFD , (struct sockaddr*) &s2 , &length );    /* Accept connection */
          
          if( !fork() ){                                                        /* Create child      */
               close( listenFD );                                               /* Close one end of the pipe */
                    printf( "Connection recved \n" );
                    int opcode = 0;
                    int retVal = 0;
                    
                    retVal = read( connectFD , &opcode , sizeof( int ) );      // Get the opCode
                    while( 0 < isDone ){
                         switch( opcode ){
                         
                         
                              default         :
                                   printf("Invalid opCode\n");
                                   break;
                         }
                         isDone = read( connectFD , &opcode , sizeof( int ) ); // Get the new opCode
                    } // End of while client actions loop
                    
                    if( isDone == -1 )
                         printf("Server>> Client connection has been lost.\n");
               close( connectFD );
               printf("Child process is done\n");
               exit(0);           /* Child exits when done */
          } // end of child process
     } // end of while
     return 0;
}


/*
     Writes output to defined logfile with date/time stamp and
     associated log level.

     Logging Levels:
     -1 : Error
     0  : Info
     1  : Debug-Warnings
     2  : Debug-All
*/
void writeLog( int loglvl , char* str ){
     // Open the log file
     int log = open( logFile, O_CREAT | O_APPEND | O_RDWR, 0664);

     // Get current date/time
     char* date = getDateString();

     // Allocate message variable
     int msgSize = strlen( str ) + strlen ( date ) + sizeof( strerror( errno ) )+ 25;
     char* msg = malloc( msgSize );

     if( loglvl == -1 ) {
          // Compose log message
          sprintf( msg, "%s\tERROR : %s\n", date, str);
          // If errno is anything other than "Success", write it to the log.
          if( errno != 0 ) {
               sprintf( msg, "%s\n", strerror( errno ) );
          }
          // Write message to log
          write( log, msg, strlen( msg ) );
     } else if(loglvl == 0 ) {
          sprintf( msg, "%s\tINFO  : %s\n", date, str);
          write( log, msg, strlen( msg ) );
     } else if( loglvl == 1 && dbgLevel >= 1 ) {
          sprintf( msg, "%s\tWARN  : %s\n", date, str);
          write( log, msg, strlen( msg ) );
     } else if( loglvl == 2 && dbgLevel >= 2 ) {
          sprintf( msg, "%s\tDEBUG : %s\n", date, str);
          write( log, msg, strlen( msg ) );
     }

     close(log);
     free(msg);
}

char* getDateString() {
     time_t t = time ( NULL );
     struct tm *timeinfo = localtime( &t );
     char* date = malloc(100);

     int year = timeinfo->tm_year + 1900;
     int month = timeinfo->tm_mon + 1;
     int day = timeinfo->tm_mday;
     int hour = timeinfo->tm_hour;
     int min = timeinfo->tm_min;
     int sec = timeinfo->tm_sec;

     sprintf( date, "[%d-%d-%d %d:%d:%d]", year, month, day, hour, min, sec);

     return date;
}