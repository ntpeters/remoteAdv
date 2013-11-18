#include "sockaddrAL.h"
#include "opCodes.h"
//#include "util.h"
#include "simplog.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

struct connection_info connectionArray[ max_connections ];
int connection_count = 0;

/*
    Adds a client connection to the array.

    Input:
    struct sockaddr_in info - defines the connection to be added
    int type                - the type of connection being added
*/
void addInfo( struct sockaddr_in info, int type, int descriptor ) {
    connection_info connection = {
        .connection_type        = type,
        .connection_descriptor  = descriptor,
        .socket_info            = info,
    };
    memcpy( &connectionArray[ connection_count ], &connection, sizeof( connection_info ) );
    connection_count++;
}

/*
    Deletes the connection matching the given sockaddr_in info.
    Unsorted list, so n time to delete.

    Input:
    struct sockaddr_in info - defines the connection to delete
*/
void deleteInfo( struct sockaddr_in info ) {
    for( int i = 0; i < connection_count ; i++ ) {
        if( !memcmp( &info, &connectionArray[i].socket_info, sizeof( struct sockaddr_in ) ) ) {
            for( int j = i; j <= connection_count; j++ ) {
                memcpy( &connectionArray[i], &connectionArray[i+1], sizeof( struct connection_info ) );
            }
            memset( &connectionArray[ connection_count - 1 ], 0, sizeof( struct connection_info ) );
            break;
        }
    }
            
    connection_count--;
}

void deleteInfoAtIndex( int index ) {
    deleteInfo( connectionArray[ index ].socket_info );
}

/*
    Gets the string representation of the client list.
    If the char buffer passed to the function is of insufficient
    size, it is realloced and the new pointer is returned.

    Input:
    char* message   - char buffer to store the returned list in
    int size        - the size of the char buffer

    Return:
    Pointer to the char buffer

*/
char* getClientListString( char* message, int size, ... ) {

    // Prepare variable length args list
    va_list args;
    va_start( args, size );
    int printFlag = (int)va_arg( args, int );
    char printType[8];

    switch( printFlag ){
    	case type_client_slave:   strcpy( printType, "slave" );   break;
    	case type_client_manager: strcpy( printType, "manager" ); break;
		case type_client_master:  strcpy( printType, "master" );  break;
		default:                  strcpy( printType, "all" );     break;
    }

	int message_size = 	( ( client_type_name_length + sizeof( int ) + client_ip_address_length ) * connection_count	) +	165;

	if( message_size > size ) {
		message = realloc( message, message_size );
	}
	
	// Write info header
	sprintf( message,
		"\n--------------------------------------------------------\n"
		"Index\tClient Type\tIP Address\tPort\tClaimed?\n"
		"--------------------------------------------------------\n"
		);

	for( int i = 0; i < connection_count; i++ ) {
		client_info client = getClientInfo( i );

		if( strcmp( client.type, printType ) && strcmp( printType, "all" ) ) {
			continue;
		}

		char* type 		= client.type;
		char* ip		= client.ip;
		int port 		= client.port;
		char* claimed 	= client.claimed;

		// Get the offset for writing the message
		int offset = strlen( message );
		// Write the data to the message
		sprintf( &message[ offset ], "%d\t%s\t\t%s\t%d\t%s\n", i, type, ip, port, claimed );
	}

	return message;
}

/*
    Gets the string representation of the client at the given index.

    Input:
    int index - index of the client to get info for

    Return:
    String representation of the designated client info
*/
char* getClientInfoString( int index ) {
    int message_size = client_type_name_length +
                       sizeof( int ) +
                       client_ip_address_length +
                       165;

    char* message = (char*)malloc(message_size);

    // Write info header
    sprintf( message,
        "\n--------------------------------------------------------\n"
        "Index\tClient Type\tIP Address\tPort\tClaimed?\n"
        "--------------------------------------------------------\n"
        );

    client_info client = getClientInfo( index );
    char* type      = client.type;
    char* ip        = client.ip;
    int port        = client.port;
    char* claimed   = client.claimed;

    // Get the offset for writing the message
    int offset = strlen( message );
    // Write the data to the message
    sprintf( &message[ offset ], "%d\t%s\t\t%s\t%d\t%s\n", index, type, ip, port, claimed );

    return message;
}

/*
    Gets the info (type, ip, port) for the client at the given index.

    Input:
    int index - index of the client to get info for

    Return:
    Struct containing relevant info about the designated client
*/
client_info getClientInfo( int index ) {
    // Convert the client type to its string form
    int connection_type = connectionArray[index].connection_type;
    char type[ client_type_name_length ];
    if( connection_type == type_client_master ) {
        strcpy( type, "master" );
    } else if( connection_type == type_client_manager ) {
        strcpy( type, "manager" );
    } else if( connection_type == type_client_slave ) {
        strcpy( type, "slave" );
    }

    // Convert the port number back from network byte order
    int port = ntohs( connectionArray[index].socket_info.sin_port );

    // Convert the IP Address back to dot notation
    char ip[16];
    int addr = connectionArray[index].socket_info.sin_addr.s_addr;
    inet_ntop( AF_INET, &addr, ip, client_ip_address_length );

    char claimed_state[4];
    int claimed = connectionArray[index].claimed;
    if( claimed == 1 ) {
        strcpy( claimed_state, "yes" );
    } else if( claimed == 0 ) {
        strcpy( claimed_state, "no" );
    }

    // Set data in tldr struct to be returned
    client_info client_ret;
    strncpy( client_ret.type, type, client_type_name_length );
    strncpy( client_ret.ip, ip, client_ip_address_length );
    client_ret.port = port;
    strcpy( client_ret.claimed, claimed_state );

    return client_ret;
}

/*
    Gets the number of currently active connections.

    Return:
    The number of currently active connections
*/
int getClientListSize(){
     return connection_count;
}

/*
    Gets the connection at the given index.

    Input:
    int index - index of the desired connection

    Return:
    The connection file descriptor
*/
int getConnection( int index ) {
    int connection = connectionArray[ index ].connection_descriptor;
    return connection;
}

/*
    Claims the client at the given index

    Input:
    int index - index of the desired client

    Return:
    0  - client successfully claimed
    -1 - client claim failed
*/
int claim( int index ) {
    if( connectionArray[ index ].claimed == 1 ) {
        return -1;
    } else {
        connectionArray[ index ].claimed = 1;
        return 0;
    }
}

/*
    Releases the client at the given index

    Input:
    ind index - index of the desired client

    Return:
    0  - client successfully released
    -1 - client release failed
*/
int release( int index ) {
    if( connectionArray[ index ].claimed == 0 ) {
        return -1;
    } else {
        connectionArray[ index ].claimed = 0;
        return 0;
    }
}
