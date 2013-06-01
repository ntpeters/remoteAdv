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
#include <stdarg.h>


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
void writeLog( int loglvl , char* str, ... );
char* getDateString();

// Output is standard out
int main( int argc , char* argv[] ){
     // Logger tests
     // writeLog( -2, "Test fatal");
     // writeLog( -1, "Test error");
     // writeLog( 0, "Test info");
     // writeLog( 1, "Test warning");
     // writeLog( 2, "Test debug");
     // writeLog( 3, "Test debug-verbose");

     
     int listenFD;
     int connectFD;
     socklen_t length;
     struct sockaddr_in s1;
     struct sockaddr_in s2;
     
     /* ---- Initialize port and print ---- */
     
     listenFD = socket( AF_INET , SOCK_STREAM , 0 );
     if( listenFD == -1 ) {
          writeLog( -2, "Open socket failed" );
          exit( 1 );
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

     writeLog( 0, "Listening on port: %d", s1.sin_port );            /* Print the port number */

     signal( SIGCHLD , SIG_IGN );                                   /* So we don't wait on zombies */
     int listenResult = listen( listenFD , 512 );                                      /* I don't think we'll hit 512 clients.. */
     if( listenResult == -1 ) {
          writeLog( -2, "Unable to listen" );
          exit(1);
     }

     /* ---- Keep listening for clients ---- */
     while( 1 ){

          length  = sizeof( s2 );
          connectFD = accept( listenFD , (struct sockaddr*) &s2 , &length );    /* Accept connection */
          if( connectFD == -1 ) {
               writeLog( -1, "Connection to client failed" );
          }
          
          if( !fork() ){                                                        /* Create child      */
               close( listenFD );                                               /* Close one end of the pipe */

               writeLog( 2, "Client connection successful");

               int opcode = 0;
                    
               int isDone = 1;
               while( isDone = read( connectFD , &opcode , sizeof( int ) ) > 0 ) {   // Get the opcode
                    switch( opcode ) {
                    
                    
                         default         :
                              writeLog( -1, "Invalid OpCode: %d", opcode );
                              break;
                    }
               } // End of while client actions loop
               
               if( isDone == -1 ) {
                    writeLog( 2, "Client connection has been lost");
               }
               
               close( connectFD );
               write( 3, "Child process is done. PID: %d", getpid() );
               exit( 0 );           /* Child exits when done */
          } // end of child process
     } // end of while

     return 0;
}


/*
     Writes output to defined logfile and standard out with
     date/time stamp and associated log level.
     
     Can take formatted string like printf, with a varialbe sized list of
     variables.

     Always adds new line to output.

     Logging Levels:
     -2 : Fatal          - A fatal error has occured: server will exit immediately
     -1 : Error          - An error has occured: server will typically not exit
     0  : Info           - Nessessary information regarding server operation
     1  : Warnings       - Any circumstance that may not affect normal operation
     2  : Debug          - Standard debug messages
     3  : Debug-Verbose  - All debug messages
*/
void writeLog( int loglvl , char* str, ... ) {
     // Open the log file
     int log = open( logFile, O_CREAT | O_APPEND | O_RDWR, 0664 );

     // Get current date/time
     char* date = getDateString();

     // Prepare variable length args lis
     va_list args;
     va_start( args, str ); 
     int max_va_list_size = 250;  // No way to determine size of list.  250 should be a good ceiling.

     // Allocate message variable
     int msgSize = strlen( str ) + strlen ( date ) + strlen( strerror( errno ) ) + 10;  // 10 char buffer to prevent overflow
     char* msg = malloc( msgSize + max_va_list_size );

    

     if( loglvl == -2 ) {
          sprintf( msg, "%s\tFATAL : ", date );
          vsprintf( msg + strlen( msg ), str, args );
          sprintf( msg + strlen( msg ), "\n" );
          // If errno is anything other than "Success", write it to the log.
          if( errno != 0 ) {
               sprintf( msg + strlen( strerror( errno ) ), "%s\n", strerror( errno ) );
          }
          // Write message to log
          write( log, msg, strlen( msg ) );
          // Write message to standard out too
          write( 0, msg, strlen( msg ) );
     } else if( loglvl == -1 ) {
          sprintf( msg, "%s\tERROR : ", date );
          vsprintf( msg + strlen( msg ), str, args );
          sprintf( msg + strlen( msg ), "\n" );
          // If errno is anything other than "Success", write it to the log.
          if( errno != 0 ) {
               sprintf( msg + strlen( strerror( errno ) ), "%s\n", strerror( errno ) );
          }
          // Write message to log
          write( log, msg, strlen( msg ) );
          // Write message to standard out too
          write( 0, msg, strlen( msg ) );
     } else if(loglvl == 0 ) {
          sprintf( msg, "%s\tINFO  : ", date );
          vsprintf( msg + strlen( msg ), str, args );
          sprintf( msg + strlen( msg ), "\n" );
          // Write message to log
          write( log, msg, strlen( msg ) );
          // Write message to standard out too
          write( 0, msg, strlen( msg ) );
     } else if( loglvl == 1 && dbgLevel >= 1 ) {
          sprintf( msg, "%s\tWARN  : ", date );
          vsprintf( msg + strlen( msg ), str, args );
          sprintf( msg + strlen( msg ), "\n" );
          // Write message to log
          write( log, msg, strlen( msg ) );
          // Write message to standard out too
          write( 0, msg, strlen( msg ) );
     } else if( loglvl == 2 && dbgLevel >= 2 ) {
          sprintf( msg, "%s\tDEBUG : ", date );
          vsprintf( msg + strlen( msg ), str, args );
          sprintf( msg + strlen( msg ), "\n" );
          // Write message to log
          write( log, msg, strlen( msg ) );
     } else if( loglvl == 3 && dbgLevel >= 3 ) {
          sprintf( msg, "%s\tDEBUG : ", date );
          vsprintf( msg + strlen( msg ), str, args );
          sprintf( msg + strlen( msg ), "\n" );
          // Write message to log
          write( log, msg, strlen( msg ) );
          // Write message to standard out too
          write( 0, msg, strlen( msg ) );
     }

     // free args list
     va_end( args );

     close( log );
     free( date );
     free( msg );
}

/*
     Gets the current date/time and returns it as a string of the form:
     [yyyy-mm-dd hh:mm:ss]
*/
char* getDateString() {
     time_t t = time ( NULL );
     struct tm *timeinfo = localtime( &t );
     char* date = malloc( 100 );

     int year = timeinfo->tm_year + 1900;
     int month = timeinfo->tm_mon + 1;
     int day = timeinfo->tm_mday;
     int hour = timeinfo->tm_hour;
     int min = timeinfo->tm_min;
     int sec = timeinfo->tm_sec;

     sprintf( date, "[%d-", year );
     // Add zero for single digit values
     if( month < 10) {
          sprintf( date + strlen( date ), "0%d-", month );
     } else {
          sprintf( date + strlen( date ), "%d-", month );
     }
     if( day < 10) {
          sprintf( date + strlen( date ), "0%d ", day );
     } else {
          sprintf( date + strlen( date ), "%d ", day );
     }
     if( hour < 10) {
          sprintf( date + strlen( date ), "0%d:", hour );
     } else {
          sprintf( date + strlen( date ), "%d:", hour );
     }
     if( min < 10) {
          sprintf( date + strlen( date ), "0%d:", min );
     } else {
          sprintf( date + strlen( date ), "%d:", min );
     }
     if( sec < 10) {
          sprintf( date + strlen( date ), "0%d]", sec );
     } else {
          sprintf( date + strlen( date ), "%d]", sec );
     }

     return date;
}
