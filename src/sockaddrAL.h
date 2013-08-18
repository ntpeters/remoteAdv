#include <sys/socket.h>
#include <netinet/in.h>
// Header of Array with functions for sockaddr_in
#ifndef SOCKADDRAL
#define SOCKADDRAL

#define max_connections				512
#define client_type_name_length		8
#define client_ip_address_length	16

typedef struct connection_info {
	int connection_type;
	int connection_descriptor;
	int claimed;
	struct sockaddr_in socket_info;
} connection_info;

typedef struct client_info {
	char type[ client_type_name_length ];
	char ip[ client_ip_address_length ];
	int port;
	char claimed[4];
} client_info;

void addInfo( struct sockaddr_in addr, int type, int descriptor );
void deleteInfo( struct sockaddr_in info );
char* getClientListString( char* message, int size, ...);
char* getClientInfoString( int index );
client_info getClientInfo( int index );
int getClientListSize();
int getConnection( int index );
int claim( int index );
int release( int index );

#endif
