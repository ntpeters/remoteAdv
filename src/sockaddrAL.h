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
	struct sockaddr_in socket_info;
} connection_info;

int getSize();
void addInfo( struct sockaddr_in addr, int type );
void deleteInfo( struct sockaddr_in info );
char* getClientList( char* message );
char* printInfo( int index );
char* getInfo( int index );

#endif
