#include "sockaddrAL.h"
#include "opCodes.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

struct connection_info connectionArray[ max_connections ];
int connection_count = 0;

int getSize(){
     return connection_count;
}

// Adds Info to the array
void addInfo( struct sockaddr_in addr, int type ) {
	connection_info connection = {
		.connection_type	= type,
		.socket_info 		= addr,
	};
	memcpy( &connectionArray[ connection_count++ ], &connection, sizeof( connection_info ) );
}

// Unsorted, so n time to delete
void deleteInfo( struct sockaddr_in info ){
     int i = 0;
     for( i = 0; i < connection_count ; connection_count++ )
          if( !memcmp( &info.sin_addr , &connectionArray[i].socket_info.sin_addr , sizeof(struct in_addr) ) )
              while( i+1 <= connection_count )  // Delete by shifting up
                   bcopy( &connectionArray[i+1] , &connectionArray[i] , sizeof( struct sockaddr_in ) );
     connection_count--;
}

char* getClientList( char* message ) {
	int message_size = 	( ( client_type_name_length +
					   		sizeof( int ) +
					   		client_ip_address_length
					   	  ) *
						  connection_count
						) +
					   	150;

	message = realloc( message, message_size );
	// Write info header
	sprintf( message,
		"\n--------------------------------------------------\n"
		"Index\tClient Type\tIP Address\tPort\n"
		"--------------------------------------------------\n"
		);

	for( int i = 0; i < connection_count; i++ ) {
		// Get the offset for writing the message
		int offset = strlen( message );
		// Write the data to the message
		sprintf( &message[ offset ], "%s\n", getInfo( i ) );
	}

	return message;
}

char* printInfo( int index ) {
	int message_size = client_type_name_length +
					   sizeof( int ) +
					   client_ip_address_length +
					   150;

	char* message = (char*)malloc(message_size);

	// Write info header
	sprintf( message,
		"\n--------------------------------------------------\n"
		"Index\tClient Type\tIP Address\tPort\n"
		"--------------------------------------------------\n"
		);

	// Get the offset for writing the message
	int offset = strlen( message );
	// Write the data to the message
	sprintf( &message[ offset ], "%s\n", getInfo( index ) );

	return message;
}

char* getInfo( int index ) {
	int message_size = client_type_name_length +
					   sizeof( int ) +
					   client_ip_address_length +
					   10;

	char* message = (char*)malloc(message_size);

	// Convert the client type to its string form
	int connection_type = connectionArray[index].connection_type;
	char type[8];
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
	inet_ntop( AF_INET, &addr, ip, 16 );

	// Write data to the message
	sprintf( message, "%d\t%s\t\t%s\t%d", index, type, ip, port );

	return message;
}