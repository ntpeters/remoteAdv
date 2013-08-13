#include "util.h"
#include <stdio.h>
#include <time.h>
#include <stdarg.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

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

     Input:
     int loglvl - The desired output logging level.  See above table for levels.
     char* str  - The message to be output. This is a format string.
     ...        - Variable length list of arguments to be used with the format string.
*/
void writeLog( int loglvl, char* str, ... ) {
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

    
     // -2: Fatal
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
     // -1: Error
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
     // 0: Info
     } else if(loglvl == 0 ) {
          sprintf( msg, "%s\tINFO  : ", date );
          vsprintf( msg + strlen( msg ), str, args );
          sprintf( msg + strlen( msg ), "\n" );
          // Write message to log
          write( log, msg, strlen( msg ) );
          // Write message to standard out too
          write( 0, msg, strlen( msg ) );
     // 1: Warning
     } else if( loglvl == 1 && dbgLevel >= 1 ) {
          sprintf( msg, "%s\tWARN  : ", date );
          vsprintf( msg + strlen( msg ), str, args );
          sprintf( msg + strlen( msg ), "\n" );
          // Write message to log
          write( log, msg, strlen( msg ) );
          // Write message to standard out too
          write( 0, msg, strlen( msg ) );
     // 2: Debug
     } else if( loglvl == 2 && dbgLevel >= 2 ) {
          sprintf( msg, "%s\tDEBUG : ", date );
          vsprintf( msg + strlen( msg ), str, args );
          sprintf( msg + strlen( msg ), "\n" );
          // Write message to log
          write( log, msg, strlen( msg ) );
          // Write message to standard out too
          write( 0, msg, strlen( msg ) );
     // 3: Verbose
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

void setDebugLevel( int level ) {
     dbgLevel = level;
}

void setLogFile( char* file ) {
     logFile = file;
}