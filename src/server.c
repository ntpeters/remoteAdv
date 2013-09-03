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
#include <stdarg.h>
#include <getopt.h>
#include <stdbool.h>

/* 141.219.153.205 for wopr     */
/* 141.219.153.206 for guardian */
/* 54.214.246.148 for my Amazon EC2 instance */

// Global Variables
int dbgLevel = dbg_verbose;
int server_portnumber = 51739;     // Port must be constant due to the nature of this project
char* logFile = "server.log";
char* version = "Development Build";
int silentMode = 0;

// Function Prototypes
void printHelp();
void handleCommand( int command, int connectFD );

// Output is standard out
int main( int argc , char* argv[] ) {

     // Handle command line arguments
     char* short_options = "d:hl:v";
     struct option long_options[] = {
          { "silent",    no_argument,        &silentMode,     1  },
          { "debug",     required_argument,  0,              'd' },
          { "help",      no_argument,        0,              'h' },
          { "logfile",   required_argument,  0,              'l' },
          { "version",   no_argument,        0,              'v' },
          { 0,           0,                  0,              0   }
     };
     int longIndex = 0;
   
     // Get first option
     char cmdOpt = getopt_long( argc, argv, short_options, long_options, &longIndex );
     while( cmdOpt != -1 ) {

          switch( cmdOpt ) {
               case 'd':
                    dbgLevel = atoi( optarg );
                    break;
               case 'l':
                    logFile = optarg;
                    break;
               case 'v':
                    printf("remoteAdv Version: %s\n", version);
                    exit(0);
               case 'h':
               case '?':             // Fall-through intentional
                    printHelp();
                    exit(0);
               default:
                    break;
          }
          // Get next option
          cmdOpt = getopt_long( argc, argv, short_options, long_options, &longIndex );
     }

     // Initilize settings based on argument input
     setDebugLevel( dbgLevel );
     setLogFile( logFile );
     setSilentMode( silentMode );


     // Begin setting up server
     writeLog( 0, "Starting remoteAdv Server - Version: %s", version );

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
               int response = type_server;
               write( connectFD, &response, sizeof( response ) );
               addInfo( s2, client_type, connectFD );
               writeLog( 3, "Client added. Descriptor: %d", connectFD );

               char* list = (char*)malloc(1000);
               list = getClientListString( list, sizeof( list ) );
          }
          
          if( !fork() ){                                                        /* Create child      */
               close( listenFD );                                               /* Close one end of the pipe */

               writeLog( 2, "Client connection successful" );
               struct client_info client = getClientInfo( getClientListSize() - 1 );
               writeLog( 3, "Connected to '%s' client at '%s:%d'", client.type, client.ip, client.port );

               int select = 0;
               int dataRead = read( connectFD , &select, sizeof( int ) );        // Get the first opcode
               
               OpHeader op;
               int command;

               while( dataRead ) {
                    switch( select ) {
                         case opcode_sent:
                              read( connectFD, &op, sizeof( op ) );
                              int connection = getConnection( 0 );
                              write( connection, &op, sizeof( op ) );
                              writeLog( 3, "Opcode '%d' sent to slave client", op.opcode );
                              break;
                         case command_sent:
                              read( connectFD, &command, sizeof( command ) );
                              writeLog( 3, "Recieved command '%d' from master client", command );
                              handleCommand( command, connectFD );
                              break;
                         default:
                              break;
                    }

                    // TODO: send response to client

                    // Get the next opcode
                    dataRead = read( connectFD , &select, sizeof( int ) );

               } // End of while client actions loop
               
               if( dataRead == -1 ) {
                    writeLog( 2, "Client connection has been lost");
               }
               
               close( connectFD );
               deleteInfo( s2 );
               writeLog( 3, "Child process is done. PID: %d", getpid() );
               exit( 0 );           /* Child exits when done */
          }
     } // end of while

     return 0;
}

/*
     Handles printing out the help dialog
*/
void printHelp() {
    printf( "remoteAdv Server - %s\n"
            "\n"
            "Usage:\tserver [arguments]\n"
            "\n"
            "Arguments:\n"
            "\t-d [level]\t\tSets the debug level:\n"
            "\t\t\t\t\t0 - No debug output\n"
            "\t\t\t\t\t1 - Warnings only (default)\n"
            "\t\t\t\t\t2 - Basic debug output\n"
            "\t\t\t\t\t3 - Verbose debug output\n"
            "\t--debug=[level]\t\tEquivalent to -d\n"
            "\t-l [filename]\t\tSets the logfile\n"
            "\t--logfile=[filename]\tEquivalent to -l\n"
            "\t-h\t\t\tDisplays this dialog\n"
            "\t--help\t\t\tEquivalent to -h\n"
            "\t--silent\t\tRestricts all output to logs only\n"
            "\t-v\t\t\tDisplays the version information of this program\n"
            "\t--version\t\tEquivalent to -v\n"
        , version );
}

/*
     Handles commands sent to the server

     Input:
     int command    - The command to execute
     int connectFD  - the connection file descriptorto the master client
*/
void handleCommand( int command, int connectFD ) {
    char* list = (char*)malloc(1000);
     memset( list, 0, 1000 );

     switch( command ) {
          case c_master_set_dbglvl:
              writeLog( 3 , "Setting Debug Level" );
              int newDbgValue;
              read( connectFD , &newDbgValue , sizeof(newDbgValue) );
              // TODO: Set global debug value on parent?
              break;
          case c_list_slaves:
               writeLog( 3, "Sending slave list" );
               list = getClientListString( list, sizeof( list ), type_client_slave );
               write( connectFD, list, strlen( list ) );
               free( list );
               break;
          case c_claim:
               writeLog( 3, "Claiming client" );
               int claim_index = -1;
               read( connectFD, &claim_index, sizeof( claim_index ) );
               int claim_result = claim( claim_index );

               if( claim_result == -1 ) {
                    writeLog( 3, "Claim failed." );
               } else {
                    writeLog( 3, "Claim succeeded." );
               }

               write( connectFD, &claim_result, sizeof( claim_result ) );
               break;
          case c_release:
               writeLog( 3, "Releasing client" );
               int release_index = -1;
               read( connectFD, &release_index, sizeof( release_index ) );
               int release_result = release( release_index );

               if( release_result == -1 ) {
                    writeLog( 3, "Release failed!" );
               } else {
                    writeLog( 3, "Release of client '%d' successful!", release_index );
               }
               break;
        case c_master_kill_client:
            writeLog( 3, "Killing slave" );

            OpHeader op = {
                .opcode = c_master_kill_client,
            };

            int kill_index = -1;
            read( connectFD, &kill_index, sizeof( kill_index ) );
            int kill_fd = getConnection( kill_index );
            write( kill_fd , &op, sizeof( op ) );

            close( kill_fd );
            deleteInfoAtIndex( kill_index );

            break;
     }
}